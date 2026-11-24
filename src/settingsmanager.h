#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

class SettingsManager : public QObject
{
    Q_OBJECT

    // Video settings
    Q_PROPERTY(QString vo READ vo WRITE setVo NOTIFY voChanged)
    Q_PROPERTY(QString gpuApi READ gpuApi WRITE setGpuApi NOTIFY gpuApiChanged)
    Q_PROPERTY(QString hwdec READ hwdec WRITE setHwdec NOTIFY hwdecChanged)
    Q_PROPERTY(QString profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(QString scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString cscale READ cscale WRITE setCscale NOTIFY cscaleChanged)
    Q_PROPERTY(bool interpolation READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)

    // Audio settings
    Q_PROPERTY(QString ao READ ao WRITE setAo NOTIFY aoChanged)
    Q_PROPERTY(QString resampleQuality READ resampleQuality WRITE setResampleQuality NOTIFY resampleQualityChanged)
    Q_PROPERTY(bool audioNormalize READ audioNormalize WRITE setAudioNormalize NOTIFY audioNormalizeChanged)
    Q_PROPERTY(int maxVolume READ maxVolume WRITE setMaxVolume NOTIFY maxVolumeChanged)

    // Subtitle & OSD
    Q_PROPERTY(QString subLanguage READ subLanguage WRITE setSubLanguage NOTIFY subLanguageChanged)
    Q_PROPERTY(QString audioLanguage READ audioLanguage WRITE setAudioLanguage NOTIFY audioLanguageChanged)
    Q_PROPERTY(int subFontSize READ subFontSize WRITE setSubFontSize NOTIFY subFontSizeChanged)
    Q_PROPERTY(bool autoSubSync READ autoSubSync WRITE setAutoSubSync NOTIFY autoSubSyncChanged)
    Q_PROPERTY(bool autoLoadNext READ autoLoadNext WRITE setAutoLoadNext NOTIFY autoLoadNextChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QString vo() const { return m_settings.value(QStringLiteral("video/vo"), QStringLiteral("gpu-next")).toString(); }
    void setVo(const QString &val) { setVal(QStringLiteral("video/vo"), val); Q_EMIT voChanged(); }

    QString gpuApi() const { return m_settings.value(QStringLiteral("video/gpuApi"), QStringLiteral("vulkan")).toString(); }
    void setGpuApi(const QString &val) { setVal(QStringLiteral("video/gpuApi"), val); Q_EMIT gpuApiChanged(); }

    QString hwdec() const { return m_settings.value(QStringLiteral("video/hwdec"), QStringLiteral("auto-safe")).toString(); }
    void setHwdec(const QString &val) { setVal(QStringLiteral("video/hwdec"), val); Q_EMIT hwdecChanged(); }

    QString profile() const { return m_settings.value(QStringLiteral("video/profile"), QStringLiteral("high-quality")).toString(); }
    void setProfile(const QString &val) { setVal(QStringLiteral("video/profile"), val); Q_EMIT profileChanged(); }

    QString scale() const { return m_settings.value(QStringLiteral("video/scale"), QStringLiteral("ewa_lanczos")).toString(); }
    void setScale(const QString &val) { setVal(QStringLiteral("video/scale"), val); Q_EMIT scaleChanged(); }

    QString cscale() const { return m_settings.value(QStringLiteral("video/cscale"), QStringLiteral("ewa_lanczos")).toString(); }
    void setCscale(const QString &val) { setVal(QStringLiteral("video/cscale"), val); Q_EMIT cscaleChanged(); }

    bool interpolation() const { return m_settings.value(QStringLiteral("video/interpolation"), false).toBool(); }
    void setInterpolation(bool val) { setVal(QStringLiteral("video/interpolation"), val); Q_EMIT interpolationChanged(); }

    QString ao() const { return m_settings.value(QStringLiteral("audio/ao"), QStringLiteral("pipewire")).toString(); }
    void setAo(const QString &val) { setVal(QStringLiteral("audio/ao"), val); Q_EMIT aoChanged(); }

    QString resampleQuality() const { return m_settings.value(QStringLiteral("audio/resample"), QStringLiteral("high")).toString(); }
    void setResampleQuality(const QString &val) { setVal(QStringLiteral("audio/resample"), val); Q_EMIT resampleQualityChanged(); }

    bool audioNormalize() const { return m_settings.value(QStringLiteral("audio/normalize"), false).toBool(); }
    void setAudioNormalize(bool val) { setVal(QStringLiteral("audio/normalize"), val); Q_EMIT audioNormalizeChanged(); }

    int maxVolume() const { return m_settings.value(QStringLiteral("audio/maxVolume"), 150).toInt(); }
    void setMaxVolume(int val) { setVal(QStringLiteral("audio/maxVolume"), val); Q_EMIT maxVolumeChanged(); }

    QString subLanguage() const { return m_settings.value(QStringLiteral("sub/language"), QStringLiteral("es,spa,en,eng")).toString(); }
    void setSubLanguage(const QString &val) { setVal(QStringLiteral("sub/language"), val); Q_EMIT subLanguageChanged(); }

    QString audioLanguage() const { return m_settings.value(QStringLiteral("audio/language"), QStringLiteral("es,spa,en,eng")).toString(); }
    void setAudioLanguage(const QString &val) { setVal(QStringLiteral("audio/language"), val); Q_EMIT audioLanguageChanged(); }

    int subFontSize() const { return m_settings.value(QStringLiteral("sub/fontSize"), 48).toInt(); }
    void setSubFontSize(int val) { setVal(QStringLiteral("sub/fontSize"), val); Q_EMIT subFontSizeChanged(); }

    bool autoSubSync() const { return m_settings.value(QStringLiteral("sub/autoSync"), true).toBool(); }
    void setAutoSubSync(bool val) { setVal(QStringLiteral("sub/autoSync"), val); Q_EMIT autoSubSyncChanged(); }

    bool autoLoadNext() const { return m_settings.value(QStringLiteral("playback/autoLoadNext"), true).toBool(); }
    void setAutoLoadNext(bool val) { setVal(QStringLiteral("playback/autoLoadNext"), val); Q_EMIT autoLoadNextChanged(); }

    Q_INVOKABLE void resetToDefaults();

Q_SIGNALS:
    void voChanged();
    void gpuApiChanged();
    void hwdecChanged();
    void profileChanged();
    void scaleChanged();
    void cscaleChanged();
    void interpolationChanged();
    void aoChanged();
    void resampleQualityChanged();
    void audioNormalizeChanged();
    void maxVolumeChanged();
    void subLanguageChanged();
    void audioLanguageChanged();
    void subFontSizeChanged();
    void autoSubSyncChanged();
    void autoLoadNextChanged();

private:
    void setVal(const QString &key, const QVariant &value) {
        m_settings.setValue(key, value);
        m_settings.sync();
    }
    QSettings m_settings{QStringLiteral("cachyos"), QStringLiteral("cachyos-player")};
};
