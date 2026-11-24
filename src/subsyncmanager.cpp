#include "subsyncmanager.h"
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QDebug>

SubSyncManager::SubSyncManager(QObject *parent)
    : QObject(parent)
{
    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SubSyncManager::onProcessFinished);
}

SubSyncManager::~SubSyncManager()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
}

void SubSyncManager::syncSubtitle(const QString &videoPath, const QString &subPath)
{
    if (m_isSyncing) return;

    QString cleanVideo = videoPath;
    if (cleanVideo.startsWith(QStringLiteral("file://"))) cleanVideo = QUrl(cleanVideo).toLocalFile();

    QString cleanSub = subPath;
    if (cleanSub.startsWith(QStringLiteral("file://"))) cleanSub = QUrl(cleanSub).toLocalFile();

    QFileInfo subInfo(cleanSub);
    if (!subInfo.exists()) {
        m_statusMessage = tr("El archivo de subtítulos no existe.");
        Q_EMIT statusMessageChanged();
        Q_EMIT syncFinished(false, QString());
        return;
    }

    m_currentVideo = cleanVideo;
    m_currentSub = cleanSub;
    m_isDownloadMode = false;

    // Output synchronized subtitle path
    m_targetOutput = QStringLiteral("%1/%2.synced.%3").arg(subInfo.path(), subInfo.completeBaseName(), subInfo.suffix());

    QString ffsubsyncBin = QStandardPaths::findExecutable(QStringLiteral("ffsubsync"));
    if (ffsubsyncBin.isEmpty()) {
        ffsubsyncBin = QDir::homePath() + QStringLiteral("/.local/bin/ffsubsync");
    }

    m_isSyncing = true;
    m_statusMessage = tr("Sincronizando subtítulos con el audio...");
    Q_EMIT syncingChanged();
    Q_EMIT statusMessageChanged();

    QStringList args;
    args << m_currentVideo << QStringLiteral("-i") << m_currentSub << QStringLiteral("-o") << m_targetOutput;

    m_process->start(ffsubsyncBin, args);
}

void SubSyncManager::downloadSubtitles(const QString &videoPath, const QString &lang)
{
    if (m_isSyncing) return;

    QString cleanVideo = videoPath;
    if (cleanVideo.startsWith(QStringLiteral("file://"))) cleanVideo = QUrl(cleanVideo).toLocalFile();

    m_currentVideo = cleanVideo;
    m_isDownloadMode = true;

    QString subliminalBin = QStandardPaths::findExecutable(QStringLiteral("subliminal"));
    if (subliminalBin.isEmpty()) {
        subliminalBin = QDir::homePath() + QStringLiteral("/.local/bin/subliminal");
    }

    m_isSyncing = true;
    m_statusMessage = tr("Buscando y descargando subtítulos...");
    Q_EMIT syncingChanged();
    Q_EMIT statusMessageChanged();

    QStringList args;
    args << QStringLiteral("download") << QStringLiteral("-l") << lang << m_currentVideo;

    m_process->start(subliminalBin, args);
}

void SubSyncManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_isSyncing = false;
    Q_EMIT syncingChanged();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        if (m_isDownloadMode) {
            m_statusMessage = tr("Subtítulos descargados correctamente.");
            Q_EMIT statusMessageChanged();
            Q_EMIT downloadFinished(true, m_currentVideo);
        } else {
            m_statusMessage = tr("Sincronización completada con éxito.");
            Q_EMIT statusMessageChanged();
            Q_EMIT syncFinished(true, m_targetOutput);
        }
    } else {
        m_statusMessage = tr("La operación no pudo completarse.");
        Q_EMIT statusMessageChanged();
        if (m_isDownloadMode) {
            Q_EMIT downloadFinished(false, QString());
        } else {
            Q_EMIT syncFinished(false, QString());
        }
    }
}
