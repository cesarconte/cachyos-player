#include "mpvcontroller.h"
#include "settingsmanager.h"
#include <clocale>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QUrl>
#include <QOpenGLContext>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <mpv/render_gl.h>

static void *get_proc_address_ctrl(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

void MpvController::on_mpv_render_update(void *ctx)
{
    MpvController *ctrl = static_cast<MpvController *>(ctx);
    QMetaObject::invokeMethod(ctrl, "mpvRedrawRequested", Qt::QueuedConnection);
}

MpvController::MpvController(QObject *parent)
    : QObject(parent)
{
    std::setlocale(LC_NUMERIC, "C");
    m_mpv = mpv_create();
    if (!m_mpv) {
        qFatal("Failed to create libmpv instance");
    }

    setupMpvOptions();

    if (mpv_initialize(m_mpv) < 0) {
        qFatal("Failed to initialize libmpv");
    }

    // Observe properties
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "path", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "track-list", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "chapter-list", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "playlist", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "playlist-pos", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "eof-reached", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "dwidth", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "dheight", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "video-aspect-override", MPV_FORMAT_DOUBLE);

    connect(&m_eventTimer, &QTimer::timeout, this, &MpvController::handleMpvEvents);
    m_eventTimer.start(25);
}

MpvController::~MpvController()
{
    m_eventTimer.stop();
    if (m_mpvGL) {
        mpv_render_context_free(m_mpvGL);
        m_mpvGL = nullptr;
    }
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

void MpvController::initRenderContext()
{
    if (m_mpvGL) return;

    mpv_opengl_init_params gl_init_params{get_proc_address_ctrl, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&m_mpvGL, m_mpv, params) < 0) {
        qWarning() << "Failed to initialize mpv render context in controller";
        return;
    }

    mpv_render_context_set_update_callback(m_mpvGL, on_mpv_render_update, this);

    if (!m_pendingFile.isEmpty()) {
        QString f = m_pendingFile;
        m_pendingFile.clear();
        openFile(f);
    }
}

void MpvController::setupMpvOptions()
{
    QSettings s(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));

    // Profile & Video options
    QString profile = s.value(QStringLiteral("video/profile"), QStringLiteral("high-quality")).toString();
    QString vo = s.value(QStringLiteral("video/vo"), QStringLiteral("gpu-next")).toString();
    QString gpuApi = s.value(QStringLiteral("video/gpuApi"), QStringLiteral("vulkan")).toString();
    QString gpuContext = s.value(QStringLiteral("video/gpuContext"), QStringLiteral("waylandvk")).toString();
    QString hwdec = s.value(QStringLiteral("video/hwdec"), QStringLiteral("auto")).toString();
    QString scale = s.value(QStringLiteral("video/scale"), QStringLiteral("ewa_lanczos")).toString();
    QString cscale = s.value(QStringLiteral("video/cscale"), QStringLiteral("lanczos")).toString();
    QString videoSync = s.value(QStringLiteral("video/videoSync"), QStringLiteral("display-resample")).toString();
    bool interpolation = s.value(QStringLiteral("video/interpolation"), true).toBool();
    bool deband = s.value(QStringLiteral("video/deband"), true).toBool();
    bool savePos = s.value(QStringLiteral("video/savePositionOnQuit"), true).toBool();
    bool resumePlay = s.value(QStringLiteral("video/resumePlayback"), false).toBool();

    mpv_set_option_string(m_mpv, "profile", profile.toUtf8().constData());
    mpv_set_option_string(m_mpv, "vo", vo.toUtf8().constData());
    mpv_set_option_string(m_mpv, "gpu-api", gpuApi.toUtf8().constData());
    mpv_set_option_string(m_mpv, "gpu-context", gpuContext.toUtf8().constData());
    mpv_set_option_string(m_mpv, "hwdec", hwdec.toUtf8().constData());
    mpv_set_option_string(m_mpv, "scale", scale.toUtf8().constData());
    mpv_set_option_string(m_mpv, "cscale", cscale.toUtf8().constData());
    mpv_set_option_string(m_mpv, "video-sync", videoSync.toUtf8().constData());
    mpv_set_option_string(m_mpv, "interpolation", interpolation ? "yes" : "no");
    mpv_set_option_string(m_mpv, "deband", deband ? "yes" : "no");
    mpv_set_option_string(m_mpv, "save-position-on-quit", savePos ? "yes" : "no");
    mpv_set_option_string(m_mpv, "resume-playback", resumePlay ? "yes" : "no");

    // Audio options
    QString ao = s.value(QStringLiteral("audio/ao"), QStringLiteral("pipewire")).toString();
    int maxVol = s.value(QStringLiteral("audio/maxVolume"), 125).toInt();
    QString audioFileAuto = s.value(QStringLiteral("audio/fileAuto"), QStringLiteral("fuzzy")).toString();
    bool gapless = s.value(QStringLiteral("audio/gapless"), true).toBool();
    bool adLavcDownmix = s.value(QStringLiteral("audio/adLavcDownmix"), false).toBool();
    int resampleSize = s.value(QStringLiteral("audio/resampleFilterSize"), 32).toInt();
    double resampleCutoff = s.value(QStringLiteral("audio/resampleCutoff"), 0.95).toDouble();
    int resamplePhase = s.value(QStringLiteral("audio/resamplePhaseShift"), 10).toInt();
    bool resampleLinear = s.value(QStringLiteral("audio/resampleLinear"), false).toBool();

    mpv_set_option_string(m_mpv, "ao", ao.toUtf8().constData());
    mpv_set_option_string(m_mpv, "volume-max", QByteArray::number(maxVol).constData());
    mpv_set_option_string(m_mpv, "gapless-audio", gapless ? "yes" : "no");
    mpv_set_option_string(m_mpv, "audio-file-auto", audioFileAuto.toUtf8().constData());
    mpv_set_option_string(m_mpv, "audio-display", "no");
    mpv_set_option_string(m_mpv, "ad-lavc-downmix", adLavcDownmix ? "yes" : "no");

    mpv_set_option_string(m_mpv, "audio-resample-filter-size", QByteArray::number(resampleSize).constData());
    mpv_set_option_string(m_mpv, "audio-resample-cutoff", QByteArray::number(resampleCutoff).constData());
    mpv_set_option_string(m_mpv, "audio-resample-phase-shift", QByteArray::number(resamplePhase).constData());
    mpv_set_option_string(m_mpv, "audio-resample-linear", resampleLinear ? "yes" : "no");

    // Subtitle options
    QString slang = s.value(QStringLiteral("sub/slang"), QStringLiteral("es-ES,es,spa")).toString();
    QString alang = s.value(QStringLiteral("sub/alang"), QStringLiteral("en,eng,es,spa")).toString();
    bool subDemuxerMkvPreroll = s.value(QStringLiteral("sub/demuxerMkvPreroll"), true).toBool();
    bool subFixTiming = s.value(QStringLiteral("sub/fixTiming"), false).toBool();
    QString subAuto = s.value(QStringLiteral("sub/auto"), QStringLiteral("fuzzy")).toString();
    bool embeddedfonts = s.value(QStringLiteral("sub/embeddedfonts"), true).toBool();
    bool subScaleWithWindow = s.value(QStringLiteral("sub/scaleWithWindow"), true).toBool();
    bool subAssScaleWithWindow = s.value(QStringLiteral("sub/assScaleWithWindow"), true).toBool();
    QString subFontProvider = s.value(QStringLiteral("sub/fontProvider"), QStringLiteral("fontconfig")).toString();
    bool subAss = s.value(QStringLiteral("sub/ass"), true).toBool();
    double subScale = s.value(QStringLiteral("sub/scale"), 1.0).toDouble();

    QString subFont = s.value(QStringLiteral("sub/font"), QStringLiteral("Netflix Sans Med")).toString();
    int subFontSize = s.value(QStringLiteral("sub/fontSize"), 54).toInt();
    QString subColor = s.value(QStringLiteral("sub/color"), QStringLiteral("#F8F8F8")).toString();
    bool subBold = s.value(QStringLiteral("sub/bold"), false).toBool();
    bool subItalic = s.value(QStringLiteral("sub/italic"), false).toBool();
    double subSpacing = s.value(QStringLiteral("sub/spacing"), 0.0).toDouble();
    double subLineSpacing = s.value(QStringLiteral("sub/lineSpacing"), 5.0).toDouble();
    QString subBorderStyle = s.value(QStringLiteral("sub/borderStyle"), QStringLiteral("outline-and-shadow")).toString();
    QString subBorderColor = s.value(QStringLiteral("sub/borderColor"), QStringLiteral("#22000000")).toString();
    double subBorderSize = s.value(QStringLiteral("sub/borderSize"), 0.8).toDouble();
    double subShadowOffset = s.value(QStringLiteral("sub/shadowOffset"), 0.25).toDouble();
    QString subShadowColor = s.value(QStringLiteral("sub/shadowColor"), QStringLiteral("#CC000000")).toString();
    double subBlur = s.value(QStringLiteral("sub/blur"), 1.0).toDouble();
    int subMarginY = s.value(QStringLiteral("sub/marginY"), 54).toInt();
    QString subAssOverride = s.value(QStringLiteral("sub/assOverride"), QStringLiteral("scale")).toString();
    bool blendSubs = s.value(QStringLiteral("sub/blendSubtitles"), true).toBool();

    mpv_set_option_string(m_mpv, "slang", slang.toUtf8().constData());
    mpv_set_option_string(m_mpv, "alang", alang.toUtf8().constData());
    mpv_set_option_string(m_mpv, "demuxer-mkv-subtitle-preroll", subDemuxerMkvPreroll ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-fix-timing", subFixTiming ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-auto", subAuto.toUtf8().constData());
    mpv_set_option_string(m_mpv, "embeddedfonts", embeddedfonts ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-scale-with-window", subScaleWithWindow ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-ass-scale-with-window", subAssScaleWithWindow ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-font-provider", subFontProvider.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-ass", subAss ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-scale", QByteArray::number(subScale).constData());

    mpv_set_option_string(m_mpv, "sub-font", subFont.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-font-size", QByteArray::number(subFontSize).constData());
    mpv_set_option_string(m_mpv, "sub-color", subColor.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-bold", subBold ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-italic", subItalic ? "yes" : "no");
    mpv_set_option_string(m_mpv, "sub-spacing", QByteArray::number(subSpacing).constData());
    mpv_set_option_string(m_mpv, "sub-line-spacing", QByteArray::number(subLineSpacing).constData());
    mpv_set_option_string(m_mpv, "sub-border-style", subBorderStyle.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-border-color", subBorderColor.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-border-size", QByteArray::number(subBorderSize).constData());
    mpv_set_option_string(m_mpv, "sub-shadow-offset", QByteArray::number(subShadowOffset).constData());
    mpv_set_option_string(m_mpv, "sub-shadow-color", subShadowColor.toUtf8().constData());
    mpv_set_option_string(m_mpv, "sub-blur", QByteArray::number(subBlur).constData());
    mpv_set_option_string(m_mpv, "sub-margin-y", QByteArray::number(subMarginY).constData());
    mpv_set_option_string(m_mpv, "sub-ass-override", subAssOverride.toUtf8().constData());
    mpv_set_option_string(m_mpv, "blend-subtitles", blendSubs ? "yes" : "no");

    // Cache, Network & Streaming
    bool cache = s.value(QStringLiteral("network/cache"), true).toBool();
    bool cachePause = s.value(QStringLiteral("network/cachePause"), false).toBool();
    bool prefetchPlaylist = s.value(QStringLiteral("network/prefetchPlaylist"), true).toBool();

    mpv_set_option_string(m_mpv, "cache", cache ? "yes" : "no");
    mpv_set_option_string(m_mpv, "cache-pause", cachePause ? "yes" : "no");
    mpv_set_option_string(m_mpv, "prefetch-playlist", prefetchPlaylist ? "yes" : "no");
    mpv_set_option_string(m_mpv, "keep-open", "yes");

    QString ytdlCookies = s.value(QStringLiteral("ytdl/cookies"),
        QStringLiteral("brave:/home/cesar/.config/BraveSoftware/Brave-Origin")).toString();
    if (!ytdlCookies.isEmpty()) {
        QString rawOpt = QStringLiteral("cookies-from-browser=%1").arg(ytdlCookies);
        mpv_set_option_string(m_mpv, "ytdl-raw-options", rawOpt.toUtf8().constData());
    }
}

void MpvController::handleMpvEvents()
{
    while (m_mpv) {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (!event || event->event_id == MPV_EVENT_NONE) {
            break;
        }

        switch (event->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            auto *prop = static_cast<mpv_event_property *>(event->data);
            if (!prop) break;

            if (strcmp(prop->name, "time-pos") == 0 && prop->format == MPV_FORMAT_DOUBLE) {
                m_position = *static_cast<double *>(prop->data);
                Q_EMIT positionChanged();
            } else if (strcmp(prop->name, "duration") == 0 && prop->format == MPV_FORMAT_DOUBLE) {
                m_duration = *static_cast<double *>(prop->data);
                Q_EMIT durationChanged();
            } else if (strcmp(prop->name, "pause") == 0 && prop->format == MPV_FORMAT_FLAG) {
                m_paused = (*static_cast<int *>(prop->data) != 0);
                Q_EMIT pausedChanged();
            } else if (strcmp(prop->name, "volume") == 0 && prop->format == MPV_FORMAT_DOUBLE) {
                m_volume = *static_cast<double *>(prop->data);
                Q_EMIT volumeChanged();
            } else if (strcmp(prop->name, "mute") == 0 && prop->format == MPV_FORMAT_FLAG) {
                m_muted = (*static_cast<int *>(prop->data) != 0);
                Q_EMIT mutedChanged();
            } else if (strcmp(prop->name, "media-title") == 0 && prop->format == MPV_FORMAT_STRING) {
                m_mediaTitle = QString::fromUtf8(*static_cast<char **>(prop->data));
                Q_EMIT mediaTitleChanged();
            } else if (strcmp(prop->name, "path") == 0 && prop->format == MPV_FORMAT_STRING) {
                m_currentPath = QString::fromUtf8(*static_cast<char **>(prop->data));
                Q_EMIT currentPathChanged();
            } else if (strcmp(prop->name, "speed") == 0 && prop->format == MPV_FORMAT_DOUBLE) {
                m_playbackSpeed = *static_cast<double *>(prop->data);
                Q_EMIT playbackSpeedChanged();
            } else if (strcmp(prop->name, "eof-reached") == 0 && prop->format == MPV_FORMAT_FLAG) {
                m_eofReached = (*static_cast<int *>(prop->data) != 0);
                Q_EMIT eofReachedChanged();
            } else if (strcmp(prop->name, "track-list") == 0) {
                updateTracks();
            } else if (strcmp(prop->name, "chapter-list") == 0) {
                updateChapters();
            } else if (strcmp(prop->name, "playlist") == 0) {
                updatePlaylist();
            } else if (strcmp(prop->name, "playlist-pos") == 0 && prop->format == MPV_FORMAT_INT64) {
                m_playlistCurrentIndex = static_cast<int>(*static_cast<int64_t *>(prop->data));
                Q_EMIT playlistCurrentIndexChanged();
            } else if (strcmp(prop->name, "dwidth") == 0 && prop->format == MPV_FORMAT_INT64) {
                m_videoWidth = static_cast<int>(*static_cast<int64_t *>(prop->data));
                if (m_videoWidth > 0 && m_videoHeight > 0) {
                    m_videoAspect = static_cast<double>(m_videoWidth) / static_cast<double>(m_videoHeight);
                }
                Q_EMIT videoSizeChanged();
            } else if (strcmp(prop->name, "dheight") == 0 && prop->format == MPV_FORMAT_INT64) {
                m_videoHeight = static_cast<int>(*static_cast<int64_t *>(prop->data));
                if (m_videoWidth > 0 && m_videoHeight > 0) {
                    m_videoAspect = static_cast<double>(m_videoWidth) / static_cast<double>(m_videoHeight);
                }
                Q_EMIT videoSizeChanged();
            }
            break;
        }
        case MPV_EVENT_FILE_LOADED: {
            updateTracks();
            updateChapters();
            updatePlaylist();
            Q_EMIT fileLoaded(m_currentPath);
            break;
        }
        default:
            break;
        }
    }
}

void MpvController::updateTracks()
{
    char *jsonStr = mpv_get_property_string(m_mpv, "track-list");
    if (!jsonStr) return;

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(jsonStr));
    mpv_free(jsonStr);

    if (doc.isArray()) {
        QVariantList list = doc.array().toVariantList();
        bool foundVideo = false;
        for (const auto &item : list) {
            QVariantMap track = item.toMap();
            if (track.value(QStringLiteral("type")).toString() == QStringLiteral("video")) {
                bool isAlbumArt = track.value(QStringLiteral("albumart"), false).toBool()
                               || track.value(QStringLiteral("image"), false).toBool();
                if (!isAlbumArt) {
                    foundVideo = true;
                    break;
                }
            }
        }
        m_trackList = list;
        Q_EMIT trackListChanged();

        if (m_hasVideo != foundVideo) {
            m_hasVideo = foundVideo;
            Q_EMIT hasVideoChanged();
        }
    }
}

void MpvController::updateChapters()
{
    if (!m_mpv) return;

    char *jsonStr = mpv_get_property_string(m_mpv, "chapter-list");
    if (!jsonStr) {
        m_chapterList.clear();
        Q_EMIT chapterListChanged();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(jsonStr));
    mpv_free(jsonStr);

    if (doc.isArray()) {
        m_chapterList = doc.array().toVariantList();
        Q_EMIT chapterListChanged();
    } else {
        m_chapterList.clear();
        Q_EMIT chapterListChanged();
    }
}

void MpvController::detectCoverArt(const QString &filePath)
{
    m_coverArtPath.clear();
    QFileInfo fi(filePath);
    if (!fi.exists()) {
        Q_EMIT coverArtPathChanged();
        return;
    }

    QDir dir = fi.dir();
    QStringList candidates = {
        QStringLiteral("cover.jpg"), QStringLiteral("cover.png"),
        QStringLiteral("folder.jpg"), QStringLiteral("folder.png"),
        QStringLiteral("front.jpg"), QStringLiteral("front.png"),
        QStringLiteral("album.jpg"), QStringLiteral("album.png"),
        fi.completeBaseName() + QStringLiteral(".jpg"),
        fi.completeBaseName() + QStringLiteral(".png")
    };

    for (const auto &cand : candidates) {
        if (dir.exists(cand)) {
            m_coverArtPath = QUrl::fromLocalFile(dir.absoluteFilePath(cand)).toString();
            break;
        }
    }

    if (m_coverArtPath.isEmpty()) {
        char *cover = mpv_get_property_string(m_mpv, "cover-art");
        if (cover) {
            m_coverArtPath = QString::fromUtf8(cover);
            mpv_free(cover);
        }
    }
    Q_EMIT coverArtPathChanged();
}

static QStringList supportedMediaFilters()
{
    return {
        QStringLiteral("*.mp4"), QStringLiteral("*.mkv"), QStringLiteral("*.avi"),
        QStringLiteral("*.mov"), QStringLiteral("*.webm"), QStringLiteral("*.m4v"),
        QStringLiteral("*.flv"), QStringLiteral("*.wmv"), QStringLiteral("*.ts"),
        QStringLiteral("*.m2ts"), QStringLiteral("*.mpg"), QStringLiteral("*.mpeg"),
        QStringLiteral("*.flac"), QStringLiteral("*.mp3"), QStringLiteral("*.ogg"),
        QStringLiteral("*.opus"), QStringLiteral("*.wav"), QStringLiteral("*.m4a"),
        QStringLiteral("*.aac"), QStringLiteral("*.wma")
    };
}

static QStringList collectMediaFiles(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists()) return {};

    if (fi.isFile()) {
        return { fi.absoluteFilePath() };
    }

    if (fi.isDir()) {
        QStringList found;
        QDirIterator it(path, supportedMediaFilters(), QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            found << it.next();
        }
        found.sort(Qt::CaseInsensitive);
        return found;
    }

    return {};
}

void MpvController::performAutoload(const QString &filePath)
{
    QSettings settings(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));
    if (!settings.value(QStringLiteral("playback/autoLoadNext"), true).toBool()) {
        return;
    }

    QFileInfo fi(filePath);
    if (!fi.exists()) return;

    QDir dir = fi.dir();
    QStringList filters = supportedMediaFilters();

    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name | QDir::LocaleAware);
    if (files.size() <= 1) return;

    // Append siblings to playlist in order
    for (const auto &file : files) {
        QString fullPath = dir.absoluteFilePath(file);
        if (fullPath != filePath) {
            const char *args[] = {"loadfile", fullPath.toUtf8().constData(), "append", nullptr};
            mpv_command_async(m_mpv, 0, args);
        }
    }
}

void MpvController::openFile(const QString &fileOrUrl)
{
    if (fileOrUrl.isEmpty()) return;

    QString cleanPath = fileOrUrl;
    if (cleanPath.startsWith(QStringLiteral("file://"))) {
        cleanPath = QUrl(cleanPath).toLocalFile();
    }

    QFileInfo fi(cleanPath);
    if (fi.isDir()) {
        QStringList mediaFiles = collectMediaFiles(cleanPath);
        if (mediaFiles.isEmpty()) return;

        if (!m_mpvGL) {
            m_pendingFile = mediaFiles.first();
            return;
        }

        detectCoverArt(mediaFiles.first());
        const char *firstArgs[] = {"loadfile", mediaFiles.first().toUtf8().constData(), "replace", nullptr};
        mpv_command_async(m_mpv, 0, firstArgs);

        for (int i = 1; i < mediaFiles.size(); ++i) {
            const char *appendArgs[] = {"loadfile", mediaFiles[i].toUtf8().constData(), "append", nullptr};
            mpv_command_async(m_mpv, 0, appendArgs);
        }
        return;
    }

    if (!m_mpvGL) {
        m_pendingFile = cleanPath;
        return;
    }

    detectCoverArt(cleanPath);

    // Save to recent files history
    SettingsManager s;
    s.addRecentFile(cleanPath);

    // Load target file first (replacing current playback)
    const char *args[] = {"loadfile", cleanPath.toUtf8().constData(), "replace", nullptr};
    mpv_command_async(m_mpv, 0, args);

    // Populate playlist with siblings in the same directory
    performAutoload(cleanPath);
}

void MpvController::openFiles(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) return;

    QStringList allMediaFiles;
    for (const auto &url : urls) {
        QString localPath = url.isLocalFile() ? url.toLocalFile() : url.toString();
        if (localPath.startsWith(QStringLiteral("file://"))) {
            localPath = QUrl(localPath).toLocalFile();
        }
        allMediaFiles << collectMediaFiles(localPath);
    }

    if (allMediaFiles.isEmpty()) return;

    if (!m_mpvGL) {
        m_pendingFile = allMediaFiles.first();
        return;
    }

    detectCoverArt(allMediaFiles.first());
    SettingsManager s;
    s.addRecentFile(allMediaFiles.first());
    const char *firstArgs[] = {"loadfile", allMediaFiles.first().toUtf8().constData(), "replace", nullptr};
    mpv_command_async(m_mpv, 0, firstArgs);

    for (int i = 1; i < allMediaFiles.size(); ++i) {
        const char *appendArgs[] = {"loadfile", allMediaFiles[i].toUtf8().constData(), "append", nullptr};
        mpv_command_async(m_mpv, 0, appendArgs);
    }
}

void MpvController::togglePause()
{
    setPaused(!m_paused);
}

void MpvController::setPaused(bool paused)
{
    int flag = paused ? 1 : 0;
    mpv_set_property_async(m_mpv, 0, "pause", MPV_FORMAT_FLAG, &flag);
}

void MpvController::stop()
{
    const char *args[] = {"stop", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::setPosition(double pos)
{
    mpv_set_property_async(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE, &pos);
}

void MpvController::seekRelative(double seconds)
{
    QByteArray secStr = QByteArray::number(seconds);
    const char *args[] = {"seek", secStr.constData(), "relative", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::setVolume(double vol)
{
    vol = qBound(0.0, vol, 150.0);
    mpv_set_property_async(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE, &vol);
}

void MpvController::setMuted(bool mute)
{
    int flag = mute ? 1 : 0;
    mpv_set_property_async(m_mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
}

void MpvController::setPlaybackSpeed(double speed)
{
    speed = qBound(0.25, speed, 4.0);
    mpv_set_property_async(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE, &speed);
}

void MpvController::nextChapter()
{
    const char *args[] = {"add", "chapter", "1", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::previousChapter()
{
    const char *args[] = {"add", "chapter", "-1", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::nextPlaylistTrack()
{
    const char *args[] = {"playlist-next", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::previousPlaylistTrack()
{
    const char *args[] = {"playlist-prev", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::selectTrack(const QString &type, int id)
{
    // mpv property names: 'sid' for subtitles, 'aid' for audio, 'vid' for video
    QByteArray propName;
    if (type == QStringLiteral("sub")) {
        propName = "sid";
    } else if (type == QStringLiteral("audio")) {
        propName = "aid";
    } else if (type == QStringLiteral("video")) {
        propName = "vid";
    } else {
        propName = type.toUtf8();
    }

    if (id < 0) {
        mpv_set_property_string(m_mpv, propName.constData(), "no");
    } else {
        QByteArray val = QByteArray::number(id);
        mpv_set_property_string(m_mpv, propName.constData(), val.constData());
    }
}

void MpvController::setProperty(const QString &name, const QVariant &value)
{
    if (value.typeId() == QMetaType::Bool) {
        int flag = value.toBool() ? 1 : 0;
        mpv_set_property_async(m_mpv, 0, name.toUtf8().constData(), MPV_FORMAT_FLAG, &flag);
    } else if (value.typeId() == QMetaType::Double || value.typeId() == QMetaType::Float) {
        double d = value.toDouble();
        mpv_set_property_async(m_mpv, 0, name.toUtf8().constData(), MPV_FORMAT_DOUBLE, &d);
    } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
        int64_t i = value.toLongLong();
        mpv_set_property_async(m_mpv, 0, name.toUtf8().constData(), MPV_FORMAT_INT64, &i);
    } else {
        QString s = value.toString();
        mpv_set_property_string(m_mpv, name.toUtf8().constData(), s.toUtf8().constData());
    }
}

QVariant MpvController::getProperty(const QString &name) const
{
    char *result = mpv_get_property_string(m_mpv, name.toUtf8().constData());
    if (!result) return QVariant();
    QString str = QString::fromUtf8(result);
    mpv_free(result);
    return str;
}

void MpvController::command(const QVariantList &args)
{
    QVector<QByteArray> utf8Args;
    QVector<const char *> cArgs;
    utf8Args.reserve(args.size());
    cArgs.reserve(args.size() + 1);

    for (const auto &arg : args) {
        utf8Args.push_back(arg.toString().toUtf8());
        cArgs.push_back(utf8Args.last().constData());
    }
    cArgs.push_back(nullptr);

    mpv_command_async(m_mpv, 0, cArgs.data());
}

void MpvController::takeScreenshot()
{
    const char *args[] = {"screenshot", "video", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::updatePlaylist()
{
    char *jsonStr = mpv_get_property_string(m_mpv, "playlist");
    if (!jsonStr) return;

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(jsonStr));
    mpv_free(jsonStr);

    if (doc.isArray()) {
        m_playlist = doc.array().toVariantList();
        Q_EMIT playlistChanged();
    }
}

void MpvController::playPlaylistItem(int index)
{
    QByteArray idx = QByteArray::number(index);
    const char *args[] = {"playlist-play-index", idx.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::removePlaylistItem(int index)
{
    QByteArray idx = QByteArray::number(index);
    const char *args[] = {"playlist-remove", idx.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvController::addFileToPlaylist(const QString &fileOrUrl)
{
    if (fileOrUrl.isEmpty()) return;
    QString clean = fileOrUrl;
    if (clean.startsWith(QStringLiteral("file://"))) clean = QUrl(clean).toLocalFile();

    QStringList mediaFiles = collectMediaFiles(clean);
    for (const auto &file : mediaFiles) {
        const char *args[] = {"loadfile", file.toUtf8().constData(), "append", nullptr};
        mpv_command_async(m_mpv, 0, args);
    }
}

void MpvController::clearPlaylist()
{
    const char *args[] = {"playlist-clear", nullptr};
    mpv_command_async(m_mpv, 0, args);
}
