#include "mprismanager.h"
#include "mpvcontroller.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

// =============================================================================
// MprisRootAdaptor
// =============================================================================

MprisRootAdaptor::MprisRootAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
}

QStringList MprisRootAdaptor::supportedUriSchemes() const
{
    return {QStringLiteral("file"), QStringLiteral("http"), QStringLiteral("https")};
}

QStringList MprisRootAdaptor::supportedMimeTypes() const
{
    return {
        QStringLiteral("video/x-matroska"), QStringLiteral("video/mp4"),
        QStringLiteral("video/webm"), QStringLiteral("video/quicktime"),
        QStringLiteral("audio/flac"), QStringLiteral("audio/mpeg"),
        QStringLiteral("audio/ogg"), QStringLiteral("audio/wav")
    };
}

void MprisRootAdaptor::Raise()
{
    Q_EMIT raiseRequested();
}

void MprisRootAdaptor::Quit()
{
    Q_EMIT quitRequested();
}

// =============================================================================
// MprisPlayerAdaptor
// =============================================================================

MprisPlayerAdaptor::MprisPlayerAdaptor(MpvController *controller, QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_ctrl(controller)
{
    setAutoRelaySignals(true);
}

QString MprisPlayerAdaptor::playbackStatus() const
{
    if (!m_ctrl || m_ctrl->currentPath().isEmpty() || m_ctrl->duration() <= 0) {
        return QStringLiteral("Stopped");
    }
    return m_ctrl->isPaused() ? QStringLiteral("Paused") : QStringLiteral("Playing");
}

double MprisPlayerAdaptor::rate() const
{
    return m_ctrl ? m_ctrl->playbackSpeed() : 1.0;
}

void MprisPlayerAdaptor::setRate(double rate)
{
    if (m_ctrl && rate > 0.0) {
        m_ctrl->setPlaybackSpeed(rate);
    }
}

QVariantMap MprisPlayerAdaptor::metadata() const
{
    QVariantMap map;
    if (!m_ctrl || m_ctrl->currentPath().isEmpty()) {
        return map;
    }

    map.insert(QStringLiteral("mpris:trackid"), QVariant::fromValue(QDBusObjectPath(QStringLiteral("/org/mpris/MediaPlayer2/Track/0"))));

    QString title = m_ctrl->mediaTitle();
    if (title.isEmpty()) {
        title = QFileInfo(m_ctrl->currentPath()).fileName();
    }
    map.insert(QStringLiteral("xesam:title"), title);

    if (m_ctrl->duration() > 0) {
        qlonglong lengthMicroseconds = static_cast<qlonglong>(m_ctrl->duration() * 1000000.0);
        map.insert(QStringLiteral("mpris:length"), lengthMicroseconds);
    }

    QString cover = m_ctrl->coverArtPath();
    if (!cover.isEmpty()) {
        map.insert(QStringLiteral("mpris:artUrl"), cover);
    }

    QString localPath = m_ctrl->currentPath();
    if (!localPath.isEmpty()) {
        if (!localPath.startsWith(QStringLiteral("http://")) && !localPath.startsWith(QStringLiteral("https://"))) {
            map.insert(QStringLiteral("xesam:url"), QUrl::fromLocalFile(localPath).toString());
        } else {
            map.insert(QStringLiteral("xesam:url"), localPath);
        }
    }

    return map;
}

double MprisPlayerAdaptor::volume() const
{
    if (!m_ctrl) return 0.0;
    return m_ctrl->isMuted() ? 0.0 : (m_ctrl->volume() / 100.0);
}

void MprisPlayerAdaptor::setVolume(double volume)
{
    if (m_ctrl) {
        m_ctrl->setVolume(qBound(0.0, volume * 100.0, 150.0));
    }
}

qlonglong MprisPlayerAdaptor::position() const
{
    if (!m_ctrl) return 0;
    return static_cast<qlonglong>(m_ctrl->position() * 1000000.0);
}

bool MprisPlayerAdaptor::canGoNext() const
{
    if (!m_ctrl) return false;
    return m_ctrl->playlist().size() > 1;
}

bool MprisPlayerAdaptor::canGoPrevious() const
{
    if (!m_ctrl) return false;
    return m_ctrl->playlist().size() > 1;
}

void MprisPlayerAdaptor::notifyPlaybackStatusChanged()
{
    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged")
    );
    signal << QStringLiteral("org.mpris.MediaPlayer2.Player");
    QVariantMap changed;
    changed.insert(QStringLiteral("PlaybackStatus"), playbackStatus());
    signal << changed;
    signal << QStringList();
    QDBusConnection::sessionBus().send(signal);
}

void MprisPlayerAdaptor::notifyMetadataChanged()
{
    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged")
    );
    signal << QStringLiteral("org.mpris.MediaPlayer2.Player");
    QVariantMap changed;
    changed.insert(QStringLiteral("Metadata"), metadata());
    signal << changed;
    signal << QStringList();
    QDBusConnection::sessionBus().send(signal);
}

void MprisPlayerAdaptor::notifyVolumeChanged()
{
    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged")
    );
    signal << QStringLiteral("org.mpris.MediaPlayer2.Player");
    QVariantMap changed;
    changed.insert(QStringLiteral("Volume"), volume());
    signal << changed;
    signal << QStringList();
    QDBusConnection::sessionBus().send(signal);
}

void MprisPlayerAdaptor::notifySeeked(qlonglong positionMicroseconds)
{
    Q_EMIT Seeked(positionMicroseconds);
}

void MprisPlayerAdaptor::Next()
{
    if (m_ctrl) m_ctrl->nextPlaylistTrack();
}

void MprisPlayerAdaptor::Previous()
{
    if (m_ctrl) m_ctrl->previousPlaylistTrack();
}

void MprisPlayerAdaptor::Pause()
{
    if (m_ctrl) m_ctrl->setPaused(true);
}

void MprisPlayerAdaptor::PlayPause()
{
    if (m_ctrl) m_ctrl->togglePause();
}

void MprisPlayerAdaptor::Stop()
{
    if (m_ctrl) m_ctrl->stop();
}

void MprisPlayerAdaptor::Play()
{
    if (m_ctrl) m_ctrl->setPaused(false);
}

void MprisPlayerAdaptor::Seek(qlonglong offsetMicroseconds)
{
    if (m_ctrl) {
        double seconds = static_cast<double>(offsetMicroseconds) / 1000000.0;
        m_ctrl->seekRelative(seconds);
    }
}

void MprisPlayerAdaptor::SetPosition(const QDBusObjectPath &trackId, qlonglong positionMicroseconds)
{
    Q_UNUSED(trackId);
    if (m_ctrl) {
        double seconds = static_cast<double>(positionMicroseconds) / 1000000.0;
        m_ctrl->setPosition(seconds);
    }
}

void MprisPlayerAdaptor::OpenUri(const QString &uri)
{
    if (m_ctrl) {
        m_ctrl->openFile(uri);
    }
}

// =============================================================================
// MprisManager
// =============================================================================

MprisManager::MprisManager(QObject *parent)
    : QObject(parent)
    , m_controller(nullptr)
{
    initDBus();
}

MprisManager::MprisManager(MpvController *controller, QObject *parent)
    : QObject(parent)
    , m_controller(controller)
{
    initDBus();
}

void MprisManager::initDBus()
{
    m_rootAdaptor = new MprisRootAdaptor(this);
    m_playerAdaptor = new MprisPlayerAdaptor(m_controller, this);

    connect(m_rootAdaptor, &MprisRootAdaptor::raiseRequested, this, &MprisManager::raiseRequested);
    connect(m_rootAdaptor, &MprisRootAdaptor::quitRequested, this, &MprisManager::quitRequested);

    auto bus = QDBusConnection::sessionBus();
    bus.registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this);
    bus.registerService(QStringLiteral("org.mpris.MediaPlayer2.cachyos_player"));

    if (m_controller) {
        setController(m_controller);
    }
}

void MprisManager::setController(MpvController *ctrl)
{
    if (m_controller == ctrl) return;

    if (m_controller) {
        disconnect(m_controller, nullptr, this, nullptr);
    }

    m_controller = ctrl;
    if (m_playerAdaptor) {
        m_playerAdaptor->setController(m_controller);
    }

    if (m_controller && m_playerAdaptor) {
        connect(m_controller, &MpvController::pausedChanged, this, [this]() {
            m_playerAdaptor->notifyPlaybackStatusChanged();
        });

        connect(m_controller, &MpvController::mediaTitleChanged, this, [this]() {
            m_playerAdaptor->notifyMetadataChanged();
        });

        connect(m_controller, &MpvController::durationChanged, this, [this]() {
            m_playerAdaptor->notifyMetadataChanged();
        });

        connect(m_controller, &MpvController::coverArtPathChanged, this, [this]() {
            m_playerAdaptor->notifyMetadataChanged();
        });

        connect(m_controller, &MpvController::currentPathChanged, this, [this]() {
            m_playerAdaptor->notifyMetadataChanged();
            m_playerAdaptor->notifyPlaybackStatusChanged();
        });

        connect(m_controller, &MpvController::volumeChanged, this, [this]() {
            m_playerAdaptor->notifyVolumeChanged();
        });

        connect(m_controller, &MpvController::mutedChanged, this, [this]() {
            m_playerAdaptor->notifyVolumeChanged();
        });
    }

    Q_EMIT controllerChanged();
}

MprisManager::~MprisManager()
{
    auto bus = QDBusConnection::sessionBus();
    bus.unregisterService(QStringLiteral("org.mpris.MediaPlayer2.cachyos_player"));
    bus.unregisterObject(QStringLiteral("/org/mpris/MediaPlayer2"));
}
