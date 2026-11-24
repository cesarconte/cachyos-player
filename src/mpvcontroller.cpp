#include "mpvcontroller.h"
#include <clocale>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>

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

    // Observe core properties
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "path", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "track-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "eof-reached", MPV_FORMAT_FLAG);

    // Event polling loop (low overhead, 25ms interval)
    connect(&m_eventTimer, &QTimer::timeout, this, &MpvController::handleMpvEvents);
    m_eventTimer.start(25);
}

MpvController::~MpvController()
{
    m_eventTimer.stop();
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

void MpvController::setupMpvOptions()
{
    QSettings settings(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));

    // Crucial: libmpv should NOT open its own independent window!
    mpv_set_option_string(m_mpv, "vo", "libmpv");

    QString hwdec = settings.value(QStringLiteral("video/hwdec"), QStringLiteral("auto-safe")).toString();
    QString scale = settings.value(QStringLiteral("video/scale"), QStringLiteral("ewa_lanczos")).toString();
    QString cscale = settings.value(QStringLiteral("video/cscale"), QStringLiteral("ewa_lanczos")).toString();
    QString ao = settings.value(QStringLiteral("audio/ao"), QStringLiteral("pipewire")).toString();
    QString resample = settings.value(QStringLiteral("audio/resample"), QStringLiteral("high")).toString();

    mpv_set_option_string(m_mpv, "hwdec", hwdec.toUtf8().constData());
    mpv_set_option_string(m_mpv, "profile", "high-quality");
    mpv_set_option_string(m_mpv, "scale", scale.toUtf8().constData());
    mpv_set_option_string(m_mpv, "cscale", cscale.toUtf8().constData());
    mpv_set_option_string(m_mpv, "dither-depth", "auto");
    mpv_set_option_string(m_mpv, "correct-downscaling", "yes");

    mpv_set_option_string(m_mpv, "ao", ao.toUtf8().constData());
    if (resample == QStringLiteral("high")) {
        mpv_set_option_string(m_mpv, "audio-resample-filter-size", "32");
        mpv_set_option_string(m_mpv, "audio-resample-cutoff", "0.98");
    }

    // Keep open at end of video & auto load audio/subtitles
    mpv_set_option_string(m_mpv, "keep-open", "yes");
    mpv_set_option_string(m_mpv, "sub-auto", "fuzzy");
    mpv_set_option_string(m_mpv, "audio-file-auto", "fuzzy");
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
            }
            break;
        }
        case MPV_EVENT_FILE_LOADED: {
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
    mpv_node node;
    if (mpv_get_property(m_mpv, "track-list", MPV_FORMAT_NODE, &node) >= 0) {
        if (node.format == MPV_FORMAT_NODE_ARRAY) {
            QVariantList list;
            bool foundVideo = false;
            for (int i = 0; i < node.u.list->num; i++) {
                if (node.u.list->values[i].format == MPV_FORMAT_NODE_MAP) {
                    QVariantMap track;
                    mpv_node_list *m = node.u.list->values[i].u.list;
                    for (int j = 0; j < m->num; j++) {
                        QString key = QString::fromUtf8(m->keys[j]);
                        if (m->values[j].format == MPV_FORMAT_STRING) {
                            track[key] = QString::fromUtf8(m->values[j].u.string);
                        } else if (m->values[j].format == MPV_FORMAT_INT64) {
                            track[key] = static_cast<qint64>(m->values[j].u.int64);
                        } else if (m->values[j].format == MPV_FORMAT_FLAG) {
                            track[key] = (m->values[j].u.flag != 0);
                        }
                    }
                    if (track.value(QStringLiteral("type")).toString() == QStringLiteral("video")) {
                        foundVideo = true;
                    }
                    list.append(track);
                }
            }
            m_trackList = list;
            Q_EMIT trackListChanged();

            if (m_hasVideo != foundVideo) {
                m_hasVideo = foundVideo;
                Q_EMIT hasVideoChanged();
            }
        }
        mpv_free_node_contents(&node);
    }
}

void MpvController::openFile(const QString &fileOrUrl)
{
    if (fileOrUrl.isEmpty()) return;

    QString cleanPath = fileOrUrl;
    if (cleanPath.startsWith(QStringLiteral("file://"))) {
        cleanPath = QUrl(cleanPath).toLocalFile();
    }

    detectCoverArt(cleanPath);
    performAutoload(cleanPath);

    const char *args[] = {"loadfile", cleanPath.toUtf8().constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
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
    QByteArray propName = type.toUtf8();
    if (id < 0) {
        const char *val = "no";
        mpv_set_property_string(m_mpv, propName.constData(), val);
    } else {
        int64_t val = id;
        mpv_set_property(m_mpv, propName.constData(), MPV_FORMAT_INT64, &val);
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
    // Also check for embedded coverart via libmpv property
    if (m_coverArtPath.isEmpty()) {
        char *cover = mpv_get_property_string(m_mpv, "cover-art");
        if (cover) {
            m_coverArtPath = QString::fromUtf8(cover);
            mpv_free(cover);
        }
    }
    Q_EMIT coverArtPathChanged();
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
    QStringList filters = {
        QStringLiteral("*.mp4"), QStringLiteral("*.mkv"), QStringLiteral("*.avi"),
        QStringLiteral("*.mov"), QStringLiteral("*.webm"), QStringLiteral("*.m4v"),
        QStringLiteral("*.flac"), QStringLiteral("*.mp3"), QStringLiteral("*.ogg"),
        QStringLiteral("*.opus"), QStringLiteral("*.wav"), QStringLiteral("*.m4a")
    };

    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name | QDir::LocaleAware);
    if (files.size() <= 1) return;

    for (const auto &file : files) {
        QString fullPath = dir.absoluteFilePath(file);
        if (fullPath != filePath) {
            const char *args[] = {"loadfile", fullPath.toUtf8().constData(), "append", nullptr};
            mpv_command_async(m_mpv, 0, args);
        }
    }
}
