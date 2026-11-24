#include "mpvcontroller.h"
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>

#include <clocale>

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
    mpv_observe_property(m_mpv, 0, "chapter-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "playlist", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "playlist-pos", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "eof-reached", MPV_FORMAT_FLAG);
}

MpvController::~MpvController()
{
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

void MpvController::setupMpvOptions()
{
    QSettings settings(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));

    // Defaults: High Quality, Vulkan, VAAPI, PipeWire
    QString vo = settings.value(QStringLiteral("video/vo"), QStringLiteral("gpu-next")).toString();
    QString gpuApi = settings.value(QStringLiteral("video/gpuApi"), QStringLiteral("vulkan")).toString();
    QString hwdec = settings.value(QStringLiteral("video/hwdec"), QStringLiteral("auto-safe")).toString();
    QString scale = settings.value(QStringLiteral("video/scale"), QStringLiteral("ewa_lanczos")).toString();
    QString cscale = settings.value(QStringLiteral("video/cscale"), QStringLiteral("ewa_lanczos")).toString();
    QString ao = settings.value(QStringLiteral("audio/ao"), QStringLiteral("pipewire")).toString();
    QString resample = settings.value(QStringLiteral("audio/resample"), QStringLiteral("high")).toString();

    mpv_set_option_string(m_mpv, "vo", vo.toUtf8().constData());
    mpv_set_option_string(m_mpv, "gpu-api", gpuApi.toUtf8().constData());
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

    // Keep open at end of video
    mpv_set_option_string(m_mpv, "keep-open", "yes");
    mpv_set_option_string(m_mpv, "sub-auto", "fuzzy");
    mpv_set_option_string(m_mpv, "audio-file-auto", "fuzzy");
}

void MpvController::openFile(const QString &fileOrUrl)
{
    if (fileOrUrl.isEmpty()) return;

    QString cleanPath = fileOrUrl;
    if (cleanPath.startsWith(QStringLiteral("file://"))) {
        cleanPath = QUrl(cleanPath).toLocalFile();
    }

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
