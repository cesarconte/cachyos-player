#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

void SettingsManager::resetToDefaults()
{
    m_settings.clear();
    m_settings.sync();

    Q_EMIT voChanged();
    Q_EMIT gpuApiChanged();
    Q_EMIT hwdecChanged();
    Q_EMIT profileChanged();
    Q_EMIT scaleChanged();
    Q_EMIT cscaleChanged();
    Q_EMIT interpolationChanged();
    Q_EMIT aoChanged();
    Q_EMIT resampleQualityChanged();
    Q_EMIT audioNormalizeChanged();
    Q_EMIT maxVolumeChanged();
    Q_EMIT subLanguageChanged();
    Q_EMIT audioLanguageChanged();
    Q_EMIT subFontSizeChanged();
    Q_EMIT autoSubSyncChanged();
    Q_EMIT autoLoadNextChanged();
}
