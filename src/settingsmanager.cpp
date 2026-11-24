#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

void SettingsManager::resetToDefaults()
{
    m_settings.clear();
    m_settings.sync();

    Q_EMIT profileChanged();
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
    Q_EMIT gaplessAudioChanged();
    Q_EMIT resampleFilterSizeChanged();
    Q_EMIT resampleCutoffChanged();
    Q_EMIT resamplePhaseShiftChanged();
    Q_EMIT audioNormalizeChanged();
    Q_EMIT slangChanged();
    Q_EMIT alangChanged();
    Q_EMIT syncToolChanged();
    Q_EMIT subFontChanged();
    Q_EMIT subFontSizeChanged();
    Q_EMIT subColorChanged();
    Q_EMIT subBorderColorChanged();
    Q_EMIT subBorderSizeChanged();
    Q_EMIT subShadowOffsetChanged();
    Q_EMIT subMarginYChanged();
    Q_EMIT subAssOverrideChanged();
    Q_EMIT blendSubtitlesChanged();
    Q_EMIT autoLoadNextChanged();
    Q_EMIT ytdlCookiesChanged();
}
