#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

class SubSyncManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isSyncing READ isSyncing NOTIFY syncingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString tool READ tool WRITE setTool NOTIFY toolChanged)

public:
    explicit SubSyncManager(QObject *parent = nullptr);
    ~SubSyncManager() override;

    bool isSyncing() const { return m_isSyncing; }
    QString statusMessage() const { return m_statusMessage; }
    QString tool() const { return m_tool; }
    void setTool(const QString &t);

    Q_INVOKABLE void syncSubtitle(const QString &videoPath, const QString &subPath);
    Q_INVOKABLE void downloadSubtitles(const QString &videoPath, const QString &lang = QStringLiteral("es"));

Q_SIGNALS:
    void syncingChanged();
    void statusMessageChanged();
    void toolChanged();
    void syncFinished(bool success, const QString &outputPath);
    void downloadFinished(bool success, const QString &downloadedSubPath);

private Q_SLOTS:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void parseAutosubCredentials(QStringList &providerArgs);

    QProcess *m_process{nullptr};
    bool m_isSyncing{false};
    QString m_statusMessage;
    QString m_tool{QStringLiteral("ffsubsync")}; // "ffsubsync" or "alass"
    QString m_currentVideo;
    QString m_currentSub;
    QString m_targetOutput;
    QString m_targetSubDir;
    QString m_videoBaseName;
    bool m_isDownloadMode{false};
};
