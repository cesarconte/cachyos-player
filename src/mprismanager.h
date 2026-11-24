#pragma once

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QVariantMap>
#include <QStringList>
#include "mpvcontroller.h"

class MprisRootAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
    Q_PROPERTY(bool CanQuit READ canQuit)
    Q_PROPERTY(bool CanRaise READ canRaise)
    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    Q_PROPERTY(QString Identity READ identity)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry)
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)

public:
    explicit MprisRootAdaptor(QObject *parent);

    bool canQuit() const { return true; }
    bool canRaise() const { return true; }
    bool hasTrackList() const { return false; }
    QString identity() const { return QStringLiteral("CachyOS Player"); }
    QString desktopEntry() const { return QStringLiteral("cachyos-player"); }
    QStringList supportedUriSchemes() const;
    QStringList supportedMimeTypes() const;

public Q_SLOTS:
    void Raise();
    void Quit();

Q_SIGNALS:
    void raiseRequested();
    void quitRequested();
};

class MprisPlayerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
    Q_PROPERTY(double Rate READ rate WRITE setRate)
    Q_PROPERTY(QVariantMap Metadata READ metadata)
    Q_PROPERTY(double Volume READ volume WRITE setVolume)
    Q_PROPERTY(qlonglong Position READ position)
    Q_PROPERTY(double MinimumRate READ minimumRate)
    Q_PROPERTY(double MaximumRate READ maximumRate)
    Q_PROPERTY(bool CanGoNext READ canGoNext)
    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
    Q_PROPERTY(bool CanPlay READ canPlay)
    Q_PROPERTY(bool CanPause READ canPause)
    Q_PROPERTY(bool CanSeek READ canSeek)
    Q_PROPERTY(bool CanControl READ canControl)

public:
    explicit MprisPlayerAdaptor(MpvController *controller, QObject *parent);

    QString playbackStatus() const;
    double rate() const;
    void setRate(double rate);
    QVariantMap metadata() const;
    double volume() const;
    void setVolume(double volume);
    qlonglong position() const;
    double minimumRate() const { return 0.25; }
    double maximumRate() const { return 4.0; }
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool canPlay() const { return true; }
    bool canPause() const { return true; }
    bool canSeek() const { return true; }
    bool canControl() const { return true; }

    void notifyPlaybackStatusChanged();
    void notifyMetadataChanged();
    void notifyVolumeChanged();
    void notifySeeked(qlonglong positionMicroseconds);
    void setController(MpvController *ctrl) { m_ctrl = ctrl; }

public Q_SLOTS:
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qlonglong offsetMicroseconds);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong positionMicroseconds);
    void OpenUri(const QString &uri);

Q_SIGNALS:
    void Seeked(qlonglong Position);

private:
    MpvController *m_ctrl;
};

class MprisManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MpvController* controller READ controller WRITE setController NOTIFY controllerChanged)

public:
    explicit MprisManager(QObject *parent = nullptr);
    explicit MprisManager(MpvController *controller, QObject *parent = nullptr);
    ~MprisManager() override;

    MpvController* controller() const { return m_controller; }
    void setController(MpvController *ctrl);

Q_SIGNALS:
    void controllerChanged();
    void raiseRequested();
    void quitRequested();

private:
    void initDBus();

    MpvController *m_controller = nullptr;
    MprisRootAdaptor *m_rootAdaptor = nullptr;
    MprisPlayerAdaptor *m_playerAdaptor = nullptr;
};
