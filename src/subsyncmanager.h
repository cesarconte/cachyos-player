#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

class SubSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isSyncing READ isSyncing NOTIFY syncingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    explicit SubSyncManager(QObject *parent = nullptr);
    ~SubSyncManager() override;

    bool isSyncing() const { return m_isSyncing; }
    QString statusMessage() const { return m_statusMessage; }

    Q_INVOKABLE void syncSubtitle(const QString &videoPath, const QString &subPath);
    Q_INVOKABLE void downloadSubtitles(const QString &videoPath, const QString &lang = QStringLiteral("es"));

Q_SIGNALS:
    void syncingChanged();
    void statusMessageChanged();
    void syncFinished(bool success, const QString &outputPath);
    void downloadFinished(bool success, const QString &downloadedSubPath);

private Q_SLOTS:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_process{nullptr};
    bool m_isSyncing{false};
    QString m_statusMessage;
    QString m_currentVideo;
    QString m_currentSub;
    QString m_targetOutput;
    bool m_isDownloadMode{false};
};
