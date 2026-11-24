#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>
#include <mpv/client.h>

class MpvController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QVariantList trackList READ trackList NOTIFY trackListChanged)
    Q_PROPERTY(QVariantList chapterList READ chapterList NOTIFY chapterListChanged)
    Q_PROPERTY(QVariantList playlist READ playlist NOTIFY playlistChanged)
    Q_PROPERTY(int playlistCurrentIndex READ playlistCurrentIndex NOTIFY playlistCurrentIndexChanged)
    Q_PROPERTY(double playbackSpeed READ playbackSpeed WRITE setPlaybackSpeed NOTIFY playbackSpeedChanged)
    Q_PROPERTY(bool eofReached READ eofReached NOTIFY eofReachedChanged)
    Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(QString coverArtPath READ coverArtPath NOTIFY coverArtPathChanged)

public:
    explicit MpvController(QObject *parent = nullptr);
    ~MpvController() override;

    mpv_handle *mpv() const { return m_mpv; }

    bool isPaused() const { return m_paused; }
    void setPaused(bool paused);

    double duration() const { return m_duration; }
    double position() const { return m_position; }
    void setPosition(double pos);

    double volume() const { return m_volume; }
    void setVolume(double vol);

    bool isMuted() const { return m_muted; }
    void setMuted(bool mute);

    QString mediaTitle() const { return m_mediaTitle; }
    QString currentPath() const { return m_currentPath; }

    QVariantList trackList() const { return m_trackList; }
    QVariantList chapterList() const { return m_chapterList; }
    QVariantList playlist() const { return m_playlist; }
    int playlistCurrentIndex() const { return m_playlistCurrentIndex; }

    double playbackSpeed() const { return m_playbackSpeed; }
    void setPlaybackSpeed(double speed);

    bool eofReached() const { return m_eofReached; }
    bool hasVideo() const { return m_hasVideo; }
    QString coverArtPath() const { return m_coverArtPath; }

    Q_INVOKABLE void openFile(const QString &fileOrUrl);
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seekRelative(double seconds);
    Q_INVOKABLE void nextChapter();
    Q_INVOKABLE void previousChapter();
    Q_INVOKABLE void nextPlaylistTrack();
    Q_INVOKABLE void previousPlaylistTrack();
    Q_INVOKABLE void selectTrack(const QString &type, int id);
    Q_INVOKABLE void setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getProperty(const QString &name) const;
    Q_INVOKABLE void command(const QVariantList &args);
    Q_INVOKABLE void takeScreenshot();

Q_SIGNALS:
    void pausedChanged();
    void durationChanged();
    void positionChanged();
    void volumeChanged();
    void mutedChanged();
    void mediaTitleChanged();
    void currentPathChanged();
    void trackListChanged();
    void chapterListChanged();
    void playlistChanged();
    void playlistCurrentIndexChanged();
    void playbackSpeedChanged();
    void eofReachedChanged();
    void hasVideoChanged();
    void coverArtPathChanged();
    void fileLoaded(const QString &path);

private Q_SLOTS:
    void handleMpvEvents();

private:
    void setupMpvOptions();
    void updateTracks();

    mpv_handle *m_mpv{nullptr};
    QTimer m_eventTimer;
    bool m_paused{false};
    double m_duration{0.0};
    double m_position{0.0};
    double m_volume{100.0};
    bool m_muted{false};
    QString m_mediaTitle;
    QString m_currentPath;
    QVariantList m_trackList;
    QVariantList m_chapterList;
    QVariantList m_playlist;
    int m_playlistCurrentIndex{-1};
    double m_playbackSpeed{1.0};
    bool m_eofReached{false};
    bool m_hasVideo{false};
    QString m_coverArtPath;
    void detectCoverArt(const QString &filePath);
    void performAutoload(const QString &filePath);
};
