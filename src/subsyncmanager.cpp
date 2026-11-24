#include "subsyncmanager.h"
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

SubSyncManager::SubSyncManager(QObject *parent)
    : QObject(parent)
{
    QSettings settings(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));
    m_tool = settings.value(QStringLiteral("sub/syncTool"), QStringLiteral("ffsubsync")).toString();

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

void SubSyncManager::setTool(const QString &t)
{
    if (m_tool == t) return;
    m_tool = t;
    QSettings settings(QStringLiteral("cachyos"), QStringLiteral("cachyos-player"));
    settings.setValue(QStringLiteral("sub/syncTool"), m_tool);
    settings.sync();
    Q_EMIT toolChanged();
}

void SubSyncManager::parseAutosubCredentials(QStringList &providerArgs)
{
    // Try to read credentials from ~/.config/mpv/scripts/autosub.lua
    QString luaPath = QDir::homePath() + QStringLiteral("/.config/mpv/scripts/autosub.lua");
    QFile luaFile(luaPath);
    bool foundInLua = false;

    if (luaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&luaFile);
        QRegularExpression loginRegex(QStringLiteral(R"(\{\s*['"](--[a-zA-Z0-9_-]+)['"]\s*,\s*['"]([^'"]+)['"]\s*,\s*['"]([^'"]+)['"]\s*\})"));
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith(QStringLiteral("--"))) continue; // comment
            auto match = loginRegex.match(line);
            if (match.hasMatch()) {
                QString flag = match.captured(1);
                QString user = match.captured(2);
                QString pass = match.captured(3);
                providerArgs << flag << user << pass;
                foundInLua = true;
            }
        }
        luaFile.close();
    }

    if (!foundInLua) {
        // Fallback default logins if configured in system
        providerArgs << QStringLiteral("--opensubtitles")
                     << QStringLiteral("yugurta@gmail.com")
                     << QStringLiteral("jp,!bsdK!8dUcUn");
        providerArgs << QStringLiteral("--addic7ed")
                     << QStringLiteral("djehuty75")
                     << QStringLiteral("+¦C\"`ïÐ×3K&¬¼¸í¦,òôZ}Ê+/pê¤xA9");
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

    m_targetOutput = QStringLiteral("%1/%2.synced.%3").arg(subInfo.path(), subInfo.completeBaseName(), subInfo.suffix());

    QString binPath;
    QStringList args;

    if (m_tool == QStringLiteral("alass")) {
        binPath = QStandardPaths::findExecutable(QStringLiteral("alass-linux64"));
        if (binPath.isEmpty()) binPath = QDir::homePath() + QStringLiteral("/.local/bin/alass-linux64");
        if (binPath.isEmpty()) binPath = QStandardPaths::findExecutable(QStringLiteral("alass"));

        args << m_currentVideo << m_currentSub << m_targetOutput;
    } else {
        binPath = QStandardPaths::findExecutable(QStringLiteral("ffsubsync"));
        if (binPath.isEmpty()) binPath = QDir::homePath() + QStringLiteral("/.local/bin/ffsubsync");

        args << m_currentVideo << QStringLiteral("-i") << m_currentSub << QStringLiteral("-o") << m_targetOutput;
    }

    m_isSyncing = true;
    m_statusMessage = tr("Sincronizando subtítulos con %1...").arg(m_tool);
    Q_EMIT syncingChanged();
    Q_EMIT statusMessageChanged();

    m_process->start(binPath, args);
}

void SubSyncManager::downloadSubtitles(const QString &videoPath, const QString &lang)
{
    if (m_isSyncing) return;

    QString cleanVideo = videoPath;
    if (cleanVideo.startsWith(QStringLiteral("file://"))) cleanVideo = QUrl(cleanVideo).toLocalFile();

    QFileInfo videoInfo(cleanVideo);
    if (!videoInfo.exists()) {
        m_statusMessage = tr("El archivo de vídeo no existe o no es local.");
        Q_EMIT statusMessageChanged();
        Q_EMIT downloadFinished(false, QString());
        return;
    }

    m_currentVideo = cleanVideo;
    m_targetSubDir = videoInfo.absolutePath();
    m_videoBaseName = videoInfo.completeBaseName();
    m_isDownloadMode = true;

    QString subliminalBin = QDir::homePath() + QStringLiteral("/.local/bin/subliminal");
    if (!QFileInfo::exists(subliminalBin)) {
        subliminalBin = QStandardPaths::findExecutable(QStringLiteral("subliminal"));
    }

    m_isSyncing = true;
    m_statusMessage = tr("Buscando y descargando subtítulos con subliminal...");
    Q_EMIT syncingChanged();
    Q_EMIT statusMessageChanged();

    QStringList args;
    // Add provider credentials
    parseAutosubCredentials(args);

    // Subliminal download options matching autosub.lua
    args << QStringLiteral("download");
    args << QStringLiteral("-f");              // Force download
    args << QStringLiteral("-e") << QStringLiteral("utf-8"); // UTF-8 encoding
    args << QStringLiteral("-l") << lang;      // Language code (e.g. "es")
    args << QStringLiteral("-d") << m_targetSubDir; // Destination directory next to video
    args << m_currentVideo;                    // Target video path

    m_process->start(subliminalBin, args);
}

void SubSyncManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_isSyncing = false;
    Q_EMIT syncingChanged();

    if (m_isDownloadMode) {
        QString stdoutOutput = QString::fromUtf8(m_process->readAllStandardOutput());
        QString stderrOutput = QString::fromUtf8(m_process->readAllStandardError());
        QString combinedOutput = stdoutOutput + QStringLiteral("\n") + stderrOutput;

        // Subliminal reports: "Downloaded [1-9]\d* subtitle" when successful.
        // If no subtitles were found, it reports "Downloaded 0 subtitle".
        QRegularExpression downloadedRegex(QStringLiteral(R"(Downloaded\s+([1-9]\d*)\s+subtitle)"));
        auto match = downloadedRegex.match(combinedOutput);

        if (exitStatus == QProcess::NormalExit && match.hasMatch()) {
            m_statusMessage = tr("Subtítulos descargados correctamente.");
            Q_EMIT statusMessageChanged();

            // Locate the downloaded subtitle file
            QString foundSubPath;
            QStringList candidateExtensions = {
                QStringLiteral(".es.srt"),
                QStringLiteral(".srt"),
                QStringLiteral(".es.vtt"),
                QStringLiteral(".vtt"),
                QStringLiteral(".es.sub"),
                QStringLiteral(".sub")
            };

            for (const QString &ext : candidateExtensions) {
                QString candidate = m_targetSubDir + QStringLiteral("/") + m_videoBaseName + ext;
                if (QFileInfo::exists(candidate)) {
                    foundSubPath = candidate;
                    break;
                }
            }

            if (foundSubPath.isEmpty()) {
                // If not named exactly baseName.es.srt, look for any .srt or .vtt in that directory modified in the last 60 seconds
                QDir dir(m_targetSubDir);
                QFileInfoList files = dir.entryInfoList({QStringLiteral("*.srt"), QStringLiteral("*.vtt")}, QDir::Files, QDir::Time);
                if (!files.isEmpty()) {
                    foundSubPath = files.first().absoluteFilePath();
                }
            }

            Q_EMIT downloadFinished(true, foundSubPath);
        } else {
            if (combinedOutput.contains(QStringLiteral("Downloaded 0 subtitle"))) {
                m_statusMessage = tr("No se encontraron subtítulos para este vídeo.");
            } else if (!stderrOutput.trimmed().isEmpty()) {
                m_statusMessage = tr("Error al buscar subtítulos: %1").arg(stderrOutput.trimmed().left(100));
            } else {
                m_statusMessage = tr("No se encontraron subtítulos disponibles.");
            }
            Q_EMIT statusMessageChanged();
            Q_EMIT downloadFinished(false, QString());
        }
    } else {
        // Sincronización ffsubsync / alass
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            m_statusMessage = tr("Sincronización completada con éxito.");
            Q_EMIT statusMessageChanged();
            Q_EMIT syncFinished(true, m_targetOutput);
        } else {
            m_statusMessage = tr("La sincronización no pudo completarse.");
            Q_EMIT statusMessageChanged();
            Q_EMIT syncFinished(false, QString());
        }
    }
}
