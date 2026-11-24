#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

class SettingsManager : public QObject
{
    Q_OBJECT

    // === 1. General & Video ===
    Q_PROPERTY(QString profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(QString gpuApi READ gpuApi WRITE setGpuApi NOTIFY gpuApiChanged)
    Q_PROPERTY(QString gpuContext READ gpuContext WRITE setGpuContext NOTIFY gpuContextChanged)
    Q_PROPERTY(QString hwdec READ hwdec WRITE setHwdec NOTIFY hwdecChanged)
    Q_PROPERTY(QString scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString cscale READ cscale WRITE setCscale NOTIFY cscaleChanged)
    Q_PROPERTY(QString videoSync READ videoSync WRITE setVideoSync NOTIFY videoSyncChanged)
    Q_PROPERTY(bool interpolation READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)
    Q_PROPERTY(bool deband READ deband WRITE setDeband NOTIFY debandChanged)
    Q_PROPERTY(bool savePositionOnQuit READ savePositionOnQuit WRITE setSavePositionOnQuit NOTIFY savePositionOnQuitChanged)
    Q_PROPERTY(bool resumePlayback READ resumePlayback WRITE setResumePlayback NOTIFY resumePlaybackChanged)

    // === 2. Audio ===
    Q_PROPERTY(QString ao READ ao WRITE setAo NOTIFY aoChanged)
    Q_PROPERTY(int maxVolume READ maxVolume WRITE setMaxVolume NOTIFY maxVolumeChanged)
    Q_PROPERTY(bool gaplessAudio READ gaplessAudio WRITE setGaplessAudio NOTIFY gaplessAudioChanged)
    Q_PROPERTY(int resampleFilterSize READ resampleFilterSize WRITE setResampleFilterSize NOTIFY resampleFilterSizeChanged)
    Q_PROPERTY(double resampleCutoff READ resampleCutoff WRITE setResampleCutoff NOTIFY resampleCutoffChanged)
    Q_PROPERTY(int resamplePhaseShift READ resamplePhaseShift WRITE setResamplePhaseShift NOTIFY resamplePhaseShiftChanged)
    Q_PROPERTY(bool audioNormalize READ audioNormalize WRITE setAudioNormalize NOTIFY audioNormalizeChanged)

    // === 3. Subtitles & Languages ===
    Q_PROPERTY(QString slang READ slang WRITE setSlang NOTIFY slangChanged)
    Q_PROPERTY(QString alang READ alang WRITE setAlang NOTIFY alangChanged)
    Q_PROPERTY(QString syncTool READ syncTool WRITE setSyncTool NOTIFY syncToolChanged)
    Q_PROPERTY(QString subFont READ subFont WRITE setSubFont NOTIFY subFontChanged)
    Q_PROPERTY(int subFontSize READ subFontSize WRITE setSubFontSize NOTIFY subFontSizeChanged)
    Q_PROPERTY(QString subColor READ subColor WRITE setSubColor NOTIFY subColorChanged)
    Q_PROPERTY(QString subBorderColor READ subBorderColor WRITE setSubBorderColor NOTIFY subBorderColorChanged)
    Q_PROPERTY(double subBorderSize READ subBorderSize WRITE setSubBorderSize NOTIFY subBorderSizeChanged)
    Q_PROPERTY(double subShadowOffset READ subShadowOffset WRITE setSubShadowOffset NOTIFY subShadowOffsetChanged)
    Q_PROPERTY(int subMarginY READ subMarginY WRITE setSubMarginY NOTIFY subMarginYChanged)
    Q_PROPERTY(QString subAssOverride READ subAssOverride WRITE setSubAssOverride NOTIFY subAssOverrideChanged)
    Q_PROPERTY(bool blendSubtitles READ blendSubtitles WRITE setBlendSubtitles NOTIFY blendSubtitlesChanged)
    Q_PROPERTY(bool autoLoadNext READ autoLoadNext WRITE setAutoLoadNext NOTIFY autoLoadNextChanged)

    // === 4. Network & Streaming (yt-dlp) ===
    Q_PROPERTY(QString ytdlCookies READ ytdlCookies WRITE setYtdlCookies NOTIFY ytdlCookiesChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    // Getters & Setters - Video
    QString profile() const { return m_settings.value(QStringLiteral("video/profile"), QStringLiteral("high-quality")).toString(); }
    void setProfile(const QString &v) { setVal(QStringLiteral("video/profile"), v); Q_EMIT profileChanged(); }

    QString gpuApi() const { return m_settings.value(QStringLiteral("video/gpuApi"), QStringLiteral("vulkan")).toString(); }
    void setGpuApi(const QString &v) { setVal(QStringLiteral("video/gpuApi"), v); Q_EMIT gpuApiChanged(); }

    QString gpuContext() const { return m_settings.value(QStringLiteral("video/gpuContext"), QStringLiteral("waylandvk")).toString(); }
    void setGpuContext(const QString &v) { setVal(QStringLiteral("video/gpuContext"), v); Q_EMIT gpuContextChanged(); }

    QString hwdec() const { return m_settings.value(QStringLiteral("video/hwdec"), QStringLiteral("auto")).toString(); }
    void setHwdec(const QString &v) { setVal(QStringLiteral("video/hwdec"), v); Q_EMIT hwdecChanged(); }

    QString scale() const { return m_settings.value(QStringLiteral("video/scale"), QStringLiteral("ewa_lanczos")).toString(); }
    void setScale(const QString &v) { setVal(QStringLiteral("video/scale"), v); Q_EMIT scaleChanged(); }

    QString cscale() const { return m_settings.value(QStringLiteral("video/cscale"), QStringLiteral("lanczos")).toString(); }
    void setCscale(const QString &v) { setVal(QStringLiteral("video/cscale"), v); Q_EMIT cscaleChanged(); }

    QString videoSync() const { return m_settings.value(QStringLiteral("video/videoSync"), QStringLiteral("display-resample")).toString(); }
    void setVideoSync(const QString &v) { setVal(QStringLiteral("video/videoSync"), v); Q_EMIT videoSyncChanged(); }

    bool interpolation() const { return m_settings.value(QStringLiteral("video/interpolation"), true).toBool(); }
    void setInterpolation(bool v) { setVal(QStringLiteral("video/interpolation"), v); Q_EMIT interpolationChanged(); }

    bool deband() const { return m_settings.value(QStringLiteral("video/deband"), true).toBool(); }
    void setDeband(bool v) { setVal(QStringLiteral("video/deband"), v); Q_EMIT debandChanged(); }

    bool savePositionOnQuit() const { return m_settings.value(QStringLiteral("video/savePositionOnQuit"), true).toBool(); }
    void setSavePositionOnQuit(bool v) { setVal(QStringLiteral("video/savePositionOnQuit"), v); Q_EMIT savePositionOnQuitChanged(); }

    bool resumePlayback() const { return m_settings.value(QStringLiteral("video/resumePlayback"), false).toBool(); }
    void setResumePlayback(bool v) { setVal(QStringLiteral("video/resumePlayback"), v); Q_EMIT resumePlaybackChanged(); }

    // Getters & Setters - Audio
    QString ao() const { return m_settings.value(QStringLiteral("audio/ao"), QStringLiteral("pipewire")).toString(); }
    void setAo(const QString &v) { setVal(QStringLiteral("audio/ao"), v); Q_EMIT aoChanged(); }

    int maxVolume() const { return m_settings.value(QStringLiteral("audio/maxVolume"), 125).toInt(); }
    void setMaxVolume(int v) { setVal(QStringLiteral("audio/maxVolume"), v); Q_EMIT maxVolumeChanged(); }

    bool gaplessAudio() const { return m_settings.value(QStringLiteral("audio/gapless"), true).toBool(); }
    void setGaplessAudio(bool v) { setVal(QStringLiteral("audio/gapless"), v); Q_EMIT gaplessAudioChanged(); }

    int resampleFilterSize() const { return m_settings.value(QStringLiteral("audio/resampleFilterSize"), 32).toInt(); }
    void setResampleFilterSize(int v) { setVal(QStringLiteral("audio/resampleFilterSize"), v); Q_EMIT resampleFilterSizeChanged(); }

    double resampleCutoff() const { return m_settings.value(QStringLiteral("audio/resampleCutoff"), 0.95).toDouble(); }
    void setResampleCutoff(double v) { setVal(QStringLiteral("audio/resampleCutoff"), v); Q_EMIT resampleCutoffChanged(); }

    int resamplePhaseShift() const { return m_settings.value(QStringLiteral("audio/resamplePhaseShift"), 10).toInt(); }
    void setResamplePhaseShift(int v) { setVal(QStringLiteral("audio/resamplePhaseShift"), v); Q_EMIT resamplePhaseShiftChanged(); }

    bool audioNormalize() const { return m_settings.value(QStringLiteral("audio/normalize"), false).toBool(); }
    void setAudioNormalize(bool v) { setVal(QStringLiteral("audio/normalize"), v); Q_EMIT audioNormalizeChanged(); }

    // Getters & Setters - Subtitles
    QString slang() const { return m_settings.value(QStringLiteral("sub/slang"), QStringLiteral("es-ES,es,spa")).toString(); }
    void setSlang(const QString &v) { setVal(QStringLiteral("sub/slang"), v); Q_EMIT slangChanged(); }

    QString alang() const { return m_settings.value(QStringLiteral("sub/alang"), QStringLiteral("en,eng,es,spa")).toString(); }
    void setAlang(const QString &v) { setVal(QStringLiteral("sub/alang"), v); Q_EMIT alangChanged(); }

    QString syncTool() const { return m_settings.value(QStringLiteral("sub/syncTool"), QStringLiteral("ffsubsync")).toString(); }
    void setSyncTool(const QString &v) { setVal(QStringLiteral("sub/syncTool"), v); Q_EMIT syncToolChanged(); }

    QString subFont() const { return m_settings.value(QStringLiteral("sub/font"), QStringLiteral("Netflix Sans Med")).toString(); }
    void setSubFont(const QString &v) { setVal(QStringLiteral("sub/font"), v); Q_EMIT subFontChanged(); }

    int subFontSize() const { return m_settings.value(QStringLiteral("sub/fontSize"), 54).toInt(); }
    void setSubFontSize(int v) { setVal(QStringLiteral("sub/fontSize"), v); Q_EMIT subFontSizeChanged(); }

    QString subColor() const { return m_settings.value(QStringLiteral("sub/color"), QStringLiteral("#F8F8F8")).toString(); }
    void setSubColor(const QString &v) { setVal(QStringLiteral("sub/color"), v); Q_EMIT subColorChanged(); }

    QString subBorderColor() const { return m_settings.value(QStringLiteral("sub/borderColor"), QStringLiteral("#22000000")).toString(); }
    void setSubBorderColor(const QString &v) { setVal(QStringLiteral("sub/borderColor"), v); Q_EMIT subBorderColorChanged(); }

    double subBorderSize() const { return m_settings.value(QStringLiteral("sub/borderSize"), 0.8).toDouble(); }
    void setSubBorderSize(double v) { setVal(QStringLiteral("sub/borderSize"), v); Q_EMIT subBorderSizeChanged(); }

    double subShadowOffset() const { return m_settings.value(QStringLiteral("sub/shadowOffset"), 0.25).toDouble(); }
    void setSubShadowOffset(double v) { setVal(QStringLiteral("sub/shadowOffset"), v); Q_EMIT subShadowOffsetChanged(); }

    int subMarginY() const { return m_settings.value(QStringLiteral("sub/marginY"), 54).toInt(); }
    void setSubMarginY(int v) { setVal(QStringLiteral("sub/marginY"), v); Q_EMIT subMarginYChanged(); }

    QString subAssOverride() const { return m_settings.value(QStringLiteral("sub/assOverride"), QStringLiteral("scale")).toString(); }
    void setSubAssOverride(const QString &v) { setVal(QStringLiteral("sub/assOverride"), v); Q_EMIT subAssOverrideChanged(); }

    bool blendSubtitles() const { return m_settings.value(QStringLiteral("sub/blendSubtitles"), true).toBool(); }
    void setBlendSubtitles(bool v) { setVal(QStringLiteral("sub/blendSubtitles"), v); Q_EMIT blendSubtitlesChanged(); }

    bool autoLoadNext() const { return m_settings.value(QStringLiteral("playback/autoLoadNext"), true).toBool(); }
    void setAutoLoadNext(bool v) { setVal(QStringLiteral("playback/autoLoadNext"), v); Q_EMIT autoLoadNextChanged(); }

    // Getters & Setters - Streaming
    QString ytdlCookies() const {
        return m_settings.value(QStringLiteral("ytdl/cookies"),
            QStringLiteral("brave:/home/cesar/.config/BraveSoftware/Brave-Origin")).toString();
    }
    void setYtdlCookies(const QString &v) { setVal(QStringLiteral("ytdl/cookies"), v); Q_EMIT ytdlCookiesChanged(); }

    Q_INVOKABLE void resetToDefaults();

Q_SIGNALS:
    void profileChanged();
    void gpuApiChanged();
    void gpuContextChanged();
    void hwdecChanged();
    void scaleChanged();
    void cscaleChanged();
    void videoSyncChanged();
    void interpolationChanged();
    void debandChanged();
    void savePositionOnQuitChanged();
    void resumePlaybackChanged();
    void aoChanged();
    void maxVolumeChanged();
    void gaplessAudioChanged();
    void resampleFilterSizeChanged();
    void resampleCutoffChanged();
    void resamplePhaseShiftChanged();
    void audioNormalizeChanged();
    void slangChanged();
    void alangChanged();
    void syncToolChanged();
    void subFontChanged();
    void subFontSizeChanged();
    void subColorChanged();
    void subBorderColorChanged();
    void subBorderSizeChanged();
    void subShadowOffsetChanged();
    void subMarginYChanged();
    void subAssOverrideChanged();
    void blendSubtitlesChanged();
    void autoLoadNextChanged();
    void ytdlCookiesChanged();

private:
    void setVal(const QString &key, const QVariant &value) {
        m_settings.setValue(key, value);
        m_settings.sync();
    }
    QSettings m_settings{QStringLiteral("cachyos"), QStringLiteral("cachyos-player")};
};
