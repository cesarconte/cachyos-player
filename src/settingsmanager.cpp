#include "settingsmanager.h"
#include <QUrl>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

void SettingsManager::resetToDefaults()
{
    m_settings.clear();
    m_settings.sync();

    Q_EMIT profileChanged();
    Q_EMIT voChanged();
    Q_EMIT gpuApiChanged();
    Q_EMIT gpuContextChanged();
    Q_EMIT hwdecChanged();
    Q_EMIT scaleChanged();
    Q_EMIT cscaleChanged();
    Q_EMIT videoSyncChanged();
    Q_EMIT interpolationChanged();
    Q_EMIT debandChanged();
    Q_EMIT savePositionOnQuitChanged();
    Q_EMIT resumePlaybackChanged();
    Q_EMIT aoChanged();
    Q_EMIT maxVolumeChanged();
    Q_EMIT audioFileAutoChanged();
    Q_EMIT audioDisplayChanged();
    Q_EMIT gaplessAudioChanged();
    Q_EMIT adLavcDownmixChanged();
    Q_EMIT resampleFilterSizeChanged();
    Q_EMIT resampleCutoffChanged();
    Q_EMIT resamplePhaseShiftChanged();
    Q_EMIT resampleLinearChanged();
    Q_EMIT audioNormalizeChanged();
    Q_EMIT slangChanged();
    Q_EMIT alangChanged();
    Q_EMIT syncToolChanged();
    Q_EMIT subDemuxerMkvPrerollChanged();
    Q_EMIT subFixTimingChanged();
    Q_EMIT subAutoChanged();
    Q_EMIT embeddedfontsChanged();
    Q_EMIT subScaleWithWindowChanged();
    Q_EMIT subAssScaleWithWindowChanged();
    Q_EMIT subFontProviderChanged();
    Q_EMIT subAssChanged();
    Q_EMIT subScaleChanged();
    Q_EMIT subFontChanged();
    Q_EMIT subFontSizeChanged();
    Q_EMIT subColorChanged();
    Q_EMIT subBoldChanged();
    Q_EMIT subItalicChanged();
    Q_EMIT subSpacingChanged();
    Q_EMIT subLineSpacingChanged();
    Q_EMIT subBorderStyleChanged();
    Q_EMIT subBorderColorChanged();
    Q_EMIT subBorderSizeChanged();
    Q_EMIT subShadowOffsetChanged();
    Q_EMIT subShadowColorChanged();
    Q_EMIT subBlurChanged();
    Q_EMIT subMarginYChanged();
    Q_EMIT subAssOverrideChanged();
    Q_EMIT blendSubtitlesChanged();
    Q_EMIT autoLoadNextChanged();
    Q_EMIT cacheChanged();
    Q_EMIT cachePauseChanged();
    Q_EMIT prefetchPlaylistChanged();
    Q_EMIT ytdlCookiesChanged();
    Q_EMIT autoSkipIntroChanged();
}

void SettingsManager::addRecentFile(const QString &fileOrFolder)
{
    if (fileOrFolder.isEmpty()) return;
    QString clean = fileOrFolder;
    if (clean.startsWith(QStringLiteral("file://"))) {
        clean = QUrl(clean).toLocalFile();
    }

    QStringList list = recentFiles();
    list.removeAll(clean);
    list.prepend(clean);
    while (list.size() > 10) {
        list.removeLast();
    }
    setVal(QStringLiteral("history/recentFiles"), list);
    Q_EMIT recentFilesChanged();
}

void SettingsManager::clearRecentFiles()
{
    setVal(QStringLiteral("history/recentFiles"), QStringList());
    Q_EMIT recentFilesChanged();
}
