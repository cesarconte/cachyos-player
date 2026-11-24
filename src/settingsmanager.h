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
    Q_PROPERTY(QString vo READ vo WRITE setVo NOTIFY voChanged)
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
    Q_PROPERTY(QString audioFileAuto READ audioFileAuto WRITE setAudioFileAuto NOTIFY audioFileAutoChanged)
    Q_PROPERTY(QString audioDisplay READ audioDisplay WRITE setAudioDisplay NOTIFY audioDisplayChanged)
    Q_PROPERTY(bool gaplessAudio READ gaplessAudio WRITE setGaplessAudio NOTIFY gaplessAudioChanged)
    Q_PROPERTY(bool adLavcDownmix READ adLavcDownmix WRITE setAdLavcDownmix NOTIFY adLavcDownmixChanged)
    Q_PROPERTY(int resampleFilterSize READ resampleFilterSize WRITE setResampleFilterSize NOTIFY resampleFilterSizeChanged)
    Q_PROPERTY(double resampleCutoff READ resampleCutoff WRITE setResampleCutoff NOTIFY resampleCutoffChanged)
    Q_PROPERTY(int resamplePhaseShift READ resamplePhaseShift WRITE setResamplePhaseShift NOTIFY resamplePhaseShiftChanged)
    Q_PROPERTY(bool resampleLinear READ resampleLinear WRITE setResampleLinear NOTIFY resampleLinearChanged)
    Q_PROPERTY(bool audioNormalize READ audioNormalize WRITE setAudioNormalize NOTIFY audioNormalizeChanged)

    // === 3. Subtitles & Languages ===
    Q_PROPERTY(QString slang READ slang WRITE setSlang NOTIFY slangChanged)
    Q_PROPERTY(QString alang READ alang WRITE setAlang NOTIFY alangChanged)
    Q_PROPERTY(QString syncTool READ syncTool WRITE setSyncTool NOTIFY syncToolChanged)
    Q_PROPERTY(bool subDemuxerMkvPreroll READ subDemuxerMkvPreroll WRITE setSubDemuxerMkvPreroll NOTIFY subDemuxerMkvPrerollChanged)
    Q_PROPERTY(bool subFixTiming READ subFixTiming WRITE setSubFixTiming NOTIFY subFixTimingChanged)
    Q_PROPERTY(QString subAuto READ subAuto WRITE setSubAuto NOTIFY subAutoChanged)
    Q_PROPERTY(bool embeddedfonts READ embeddedfonts WRITE setEmbeddedfonts NOTIFY embeddedfontsChanged)
    Q_PROPERTY(bool subScaleWithWindow READ subScaleWithWindow WRITE setSubScaleWithWindow NOTIFY subScaleWithWindowChanged)
    Q_PROPERTY(bool subAssScaleWithWindow READ subAssScaleWithWindow WRITE setSubAssScaleWithWindow NOTIFY subAssScaleWithWindowChanged)
    Q_PROPERTY(QString subFontProvider READ subFontProvider WRITE setSubFontProvider NOTIFY subFontProviderChanged)
    Q_PROPERTY(bool subAss READ subAss WRITE setSubAss NOTIFY subAssChanged)
    Q_PROPERTY(double subScale READ subScale WRITE setSubScale NOTIFY subScaleChanged)
    Q_PROPERTY(QString subFont READ subFont WRITE setSubFont NOTIFY subFontChanged)
    Q_PROPERTY(int subFontSize READ subFontSize WRITE setSubFontSize NOTIFY subFontSizeChanged)
    Q_PROPERTY(QString subColor READ subColor WRITE setSubColor NOTIFY subColorChanged)
    Q_PROPERTY(bool subBold READ subBold WRITE setSubBold NOTIFY subBoldChanged)
    Q_PROPERTY(bool subItalic READ subItalic WRITE setSubItalic NOTIFY subItalicChanged)
    Q_PROPERTY(double subSpacing READ subSpacing WRITE setSubSpacing NOTIFY subSpacingChanged)
    Q_PROPERTY(double subLineSpacing READ subLineSpacing WRITE setSubLineSpacing NOTIFY subLineSpacingChanged)
    Q_PROPERTY(QString subBorderStyle READ subBorderStyle WRITE setSubBorderStyle NOTIFY subBorderStyleChanged)
    Q_PROPERTY(QString subBorderColor READ subBorderColor WRITE setSubBorderColor NOTIFY subBorderColorChanged)
    Q_PROPERTY(double subBorderSize READ subBorderSize WRITE setSubBorderSize NOTIFY subBorderSizeChanged)
    Q_PROPERTY(double subShadowOffset READ subShadowOffset WRITE setSubShadowOffset NOTIFY subShadowOffsetChanged)
    Q_PROPERTY(QString subShadowColor READ subShadowColor WRITE setSubShadowColor NOTIFY subShadowColorChanged)
    Q_PROPERTY(double subBlur READ subBlur WRITE setSubBlur NOTIFY subBlurChanged)
    Q_PROPERTY(int subMarginY READ subMarginY WRITE setSubMarginY NOTIFY subMarginYChanged)
    Q_PROPERTY(QString subAssOverride READ subAssOverride WRITE setSubAssOverride NOTIFY subAssOverrideChanged)
    Q_PROPERTY(bool blendSubtitles READ blendSubtitles WRITE setBlendSubtitles NOTIFY blendSubtitlesChanged)
    Q_PROPERTY(bool autoLoadNext READ autoLoadNext WRITE setAutoLoadNext NOTIFY autoLoadNextChanged)

    // === 4. Network, Cache & Streaming ===
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(bool cachePause READ cachePause WRITE setCachePause NOTIFY cachePauseChanged)
    Q_PROPERTY(bool prefetchPlaylist READ prefetchPlaylist WRITE setPrefetchPlaylist NOTIFY prefetchPlaylistChanged)
    Q_PROPERTY(QString ytdlCookies READ ytdlCookies WRITE setYtdlCookies NOTIFY ytdlCookiesChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    // Getters & Setters - Video
    QString profile() const { return m_settings.value(QStringLiteral("video/profile"), QStringLiteral("high-quality")).toString(); }
    void setProfile(const QString &v) { setVal(QStringLiteral("video/profile"), v); Q_EMIT profileChanged(); }

    QString vo() const { return m_settings.value(QStringLiteral("video/vo"), QStringLiteral("gpu-next")).toString(); }
    void setVo(const QString &v) { setVal(QStringLiteral("video/vo"), v); Q_EMIT voChanged(); }

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

    QString audioFileAuto() const { return m_settings.value(QStringLiteral("audio/fileAuto"), QStringLiteral("fuzzy")).toString(); }
    void setAudioFileAuto(const QString &v) { setVal(QStringLiteral("audio/fileAuto"), v); Q_EMIT audioFileAutoChanged(); }

    QString audioDisplay() const { return m_settings.value(QStringLiteral("audio/display"), QStringLiteral("embedded-first")).toString(); }
    void setAudioDisplay(const QString &v) { setVal(QStringLiteral("audio/display"), v); Q_EMIT audioDisplayChanged(); }

    bool gaplessAudio() const { return m_settings.value(QStringLiteral("audio/gapless"), true).toBool(); }
    void setGaplessAudio(bool v) { setVal(QStringLiteral("audio/gapless"), v); Q_EMIT gaplessAudioChanged(); }

    bool adLavcDownmix() const { return m_settings.value(QStringLiteral("audio/adLavcDownmix"), false).toBool(); }
    void setAdLavcDownmix(bool v) { setVal(QStringLiteral("audio/adLavcDownmix"), v); Q_EMIT adLavcDownmixChanged(); }

    int resampleFilterSize() const { return m_settings.value(QStringLiteral("audio/resampleFilterSize"), 32).toInt(); }
    void setResampleFilterSize(int v) { setVal(QStringLiteral("audio/resampleFilterSize"), v); Q_EMIT resampleFilterSizeChanged(); }

    double resampleCutoff() const { return m_settings.value(QStringLiteral("audio/resampleCutoff"), 0.95).toDouble(); }
    void setResampleCutoff(double v) { setVal(QStringLiteral("audio/resampleCutoff"), v); Q_EMIT resampleCutoffChanged(); }

    int resamplePhaseShift() const { return m_settings.value(QStringLiteral("audio/resamplePhaseShift"), 10).toInt(); }
    void setResamplePhaseShift(int v) { setVal(QStringLiteral("audio/resamplePhaseShift"), v); Q_EMIT resamplePhaseShiftChanged(); }

    bool resampleLinear() const { return m_settings.value(QStringLiteral("audio/resampleLinear"), false).toBool(); }
    void setResampleLinear(bool v) { setVal(QStringLiteral("audio/resampleLinear"), v); Q_EMIT resampleLinearChanged(); }

    bool audioNormalize() const { return m_settings.value(QStringLiteral("audio/normalize"), false).toBool(); }
    void setAudioNormalize(bool v) { setVal(QStringLiteral("audio/normalize"), v); Q_EMIT audioNormalizeChanged(); }

    // Getters & Setters - Subtitles
    QString slang() const { return m_settings.value(QStringLiteral("sub/slang"), QStringLiteral("es-ES,es,spa")).toString(); }
    void setSlang(const QString &v) { setVal(QStringLiteral("sub/slang"), v); Q_EMIT slangChanged(); }

    QString alang() const { return m_settings.value(QStringLiteral("sub/alang"), QStringLiteral("en,eng,es,spa")).toString(); }
    void setAlang(const QString &v) { setVal(QStringLiteral("sub/alang"), v); Q_EMIT alangChanged(); }

    QString syncTool() const { return m_settings.value(QStringLiteral("sub/syncTool"), QStringLiteral("ffsubsync")).toString(); }
    void setSyncTool(const QString &v) { setVal(QStringLiteral("sub/syncTool"), v); Q_EMIT syncToolChanged(); }

    bool subDemuxerMkvPreroll() const { return m_settings.value(QStringLiteral("sub/demuxerMkvPreroll"), true).toBool(); }
    void setSubDemuxerMkvPreroll(bool v) { setVal(QStringLiteral("sub/demuxerMkvPreroll"), v); Q_EMIT subDemuxerMkvPrerollChanged(); }

    bool subFixTiming() const { return m_settings.value(QStringLiteral("sub/fixTiming"), false).toBool(); }
    void setSubFixTiming(bool v) { setVal(QStringLiteral("sub/fixTiming"), v); Q_EMIT subFixTimingChanged(); }

    QString subAuto() const { return m_settings.value(QStringLiteral("sub/auto"), QStringLiteral("fuzzy")).toString(); }
    void setSubAuto(const QString &v) { setVal(QStringLiteral("sub/auto"), v); Q_EMIT subAutoChanged(); }

    bool embeddedfonts() const { return m_settings.value(QStringLiteral("sub/embeddedfonts"), true).toBool(); }
    void setEmbeddedfonts(bool v) { setVal(QStringLiteral("sub/embeddedfonts"), v); Q_EMIT embeddedfontsChanged(); }

    bool subScaleWithWindow() const { return m_settings.value(QStringLiteral("sub/scaleWithWindow"), true).toBool(); }
    void setSubScaleWithWindow(bool v) { setVal(QStringLiteral("sub/scaleWithWindow"), v); Q_EMIT subScaleWithWindowChanged(); }

    bool subAssScaleWithWindow() const { return m_settings.value(QStringLiteral("sub/assScaleWithWindow"), true).toBool(); }
    void setSubAssScaleWithWindow(bool v) { setVal(QStringLiteral("sub/assScaleWithWindow"), v); Q_EMIT subAssScaleWithWindowChanged(); }

    QString subFontProvider() const { return m_settings.value(QStringLiteral("sub/fontProvider"), QStringLiteral("fontconfig")).toString(); }
    void setSubFontProvider(const QString &v) { setVal(QStringLiteral("sub/fontProvider"), v); Q_EMIT subFontProviderChanged(); }

    bool subAss() const { return m_settings.value(QStringLiteral("sub/ass"), true).toBool(); }
    void setSubAss(bool v) { setVal(QStringLiteral("sub/ass"), v); Q_EMIT subAssChanged(); }

    double subScale() const { return m_settings.value(QStringLiteral("sub/scale"), 1.0).toDouble(); }
    void setSubScale(double v) { setVal(QStringLiteral("sub/scale"), v); Q_EMIT subScaleChanged(); }

    QString subFont() const { return m_settings.value(QStringLiteral("sub/font"), QStringLiteral("Netflix Sans Med")).toString(); }
    void setSubFont(const QString &v) { setVal(QStringLiteral("sub/font"), v); Q_EMIT subFontChanged(); }

    int subFontSize() const { return m_settings.value(QStringLiteral("sub/fontSize"), 54).toInt(); }
    void setSubFontSize(int v) { setVal(QStringLiteral("sub/fontSize"), v); Q_EMIT subFontSizeChanged(); }

    QString subColor() const { return m_settings.value(QStringLiteral("sub/color"), QStringLiteral("#F8F8F8")).toString(); }
    void setSubColor(const QString &v) { setVal(QStringLiteral("sub/color"), v); Q_EMIT subColorChanged(); }

    bool subBold() const { return m_settings.value(QStringLiteral("sub/bold"), false).toBool(); }
    void setSubBold(bool v) { setVal(QStringLiteral("sub/bold"), v); Q_EMIT subBoldChanged(); }

    bool subItalic() const { return m_settings.value(QStringLiteral("sub/italic"), false).toBool(); }
    void setSubItalic(bool v) { setVal(QStringLiteral("sub/italic"), v); Q_EMIT subItalicChanged(); }

    double subSpacing() const { return m_settings.value(QStringLiteral("sub/spacing"), 0.0).toDouble(); }
    void setSubSpacing(double v) { setVal(QStringLiteral("sub/spacing"), v); Q_EMIT subSpacingChanged(); }

    double subLineSpacing() const { return m_settings.value(QStringLiteral("sub/lineSpacing"), 5.0).toDouble(); }
    void setSubLineSpacing(double v) { setVal(QStringLiteral("sub/lineSpacing"), v); Q_EMIT subLineSpacingChanged(); }

    QString subBorderStyle() const { return m_settings.value(QStringLiteral("sub/borderStyle"), QStringLiteral("outline-and-shadow")).toString(); }
    void setSubBorderStyle(const QString &v) { setVal(QStringLiteral("sub/borderStyle"), v); Q_EMIT subBorderStyleChanged(); }

    QString subBorderColor() const { return m_settings.value(QStringLiteral("sub/borderColor"), QStringLiteral("#22000000")).toString(); }
    void setSubBorderColor(const QString &v) { setVal(QStringLiteral("sub/borderColor"), v); Q_EMIT subBorderColorChanged(); }

    double subBorderSize() const { return m_settings.value(QStringLiteral("sub/borderSize"), 0.8).toDouble(); }
    void setSubBorderSize(double v) { setVal(QStringLiteral("sub/borderSize"), v); Q_EMIT subBorderSizeChanged(); }

    double subShadowOffset() const { return m_settings.value(QStringLiteral("sub/shadowOffset"), 0.25).toDouble(); }
    void setSubShadowOffset(double v) { setVal(QStringLiteral("sub/shadowOffset"), v); Q_EMIT subShadowOffsetChanged(); }

    QString subShadowColor() const { return m_settings.value(QStringLiteral("sub/shadowColor"), QStringLiteral("#CC000000")).toString(); }
    void setSubShadowColor(const QString &v) { setVal(QStringLiteral("sub/shadowColor"), v); Q_EMIT subShadowColorChanged(); }

    double subBlur() const { return m_settings.value(QStringLiteral("sub/blur"), 1.0).toDouble(); }
    void setSubBlur(double v) { setVal(QStringLiteral("sub/blur"), v); Q_EMIT subBlurChanged(); }

    int subMarginY() const { return m_settings.value(QStringLiteral("sub/marginY"), 54).toInt(); }
    void setSubMarginY(int v) { setVal(QStringLiteral("sub/marginY"), v); Q_EMIT subMarginYChanged(); }

    QString subAssOverride() const { return m_settings.value(QStringLiteral("sub/assOverride"), QStringLiteral("scale")).toString(); }
    void setSubAssOverride(const QString &v) { setVal(QStringLiteral("sub/assOverride"), v); Q_EMIT subAssOverrideChanged(); }

    bool blendSubtitles() const { return m_settings.value(QStringLiteral("sub/blendSubtitles"), true).toBool(); }
    void setBlendSubtitles(bool v) { setVal(QStringLiteral("sub/blendSubtitles"), v); Q_EMIT blendSubtitlesChanged(); }

    bool autoLoadNext() const { return m_settings.value(QStringLiteral("playback/autoLoadNext"), true).toBool(); }
    void setAutoLoadNext(bool v) { setVal(QStringLiteral("playback/autoLoadNext"), v); Q_EMIT autoLoadNextChanged(); }

    // Getters & Setters - Cache, Network & Streaming
    bool cache() const { return m_settings.value(QStringLiteral("network/cache"), true).toBool(); }
    void setCache(bool v) { setVal(QStringLiteral("network/cache"), v); Q_EMIT cacheChanged(); }

    bool cachePause() const { return m_settings.value(QStringLiteral("network/cachePause"), false).toBool(); }
    void setCachePause(bool v) { setVal(QStringLiteral("network/cachePause"), v); Q_EMIT cachePauseChanged(); }

    bool prefetchPlaylist() const { return m_settings.value(QStringLiteral("network/prefetchPlaylist"), true).toBool(); }
    void setPrefetchPlaylist(bool v) { setVal(QStringLiteral("network/prefetchPlaylist"), v); Q_EMIT prefetchPlaylistChanged(); }

    // Getters & Setters - Streaming
    QString ytdlCookies() const {
        return m_settings.value(QStringLiteral("ytdl/cookies"),
            QStringLiteral("brave:/home/cesar/.config/BraveSoftware/Brave-Origin")).toString();
    }
    void setYtdlCookies(const QString &v) { setVal(QStringLiteral("ytdl/cookies"), v); Q_EMIT ytdlCookiesChanged(); }

    Q_INVOKABLE void resetToDefaults();

Q_SIGNALS:
    void profileChanged();
    void voChanged();
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
    void audioFileAutoChanged();
    void audioDisplayChanged();
    void gaplessAudioChanged();
    void adLavcDownmixChanged();
    void resampleFilterSizeChanged();
    void resampleCutoffChanged();
    void resamplePhaseShiftChanged();
    void resampleLinearChanged();
    void audioNormalizeChanged();
    void slangChanged();
    void alangChanged();
    void syncToolChanged();
    void subDemuxerMkvPrerollChanged();
    void subFixTimingChanged();
    void subAutoChanged();
    void embeddedfontsChanged();
    void subScaleWithWindowChanged();
    void subAssScaleWithWindowChanged();
    void subFontProviderChanged();
    void subAssChanged();
    void subScaleChanged();
    void subFontChanged();
    void subFontSizeChanged();
    void subColorChanged();
    void subBoldChanged();
    void subItalicChanged();
    void subSpacingChanged();
    void subLineSpacingChanged();
    void subBorderStyleChanged();
    void subBorderColorChanged();
    void subBorderSizeChanged();
    void subShadowOffsetChanged();
    void subShadowColorChanged();
    void subBlurChanged();
    void subMarginYChanged();
    void subAssOverrideChanged();
    void blendSubtitlesChanged();
    void autoLoadNextChanged();
    void cacheChanged();
    void cachePauseChanged();
    void prefetchPlaylistChanged();
    void ytdlCookiesChanged();

private:
    void setVal(const QString &key, const QVariant &value) {
        m_settings.setValue(key, value);
        m_settings.sync();
    }
    QSettings m_settings{QStringLiteral("cachyos"), QStringLiteral("cachyos-player")};
};
