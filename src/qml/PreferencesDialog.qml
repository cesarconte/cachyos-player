import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

ModalSheet {
    id: root
    title: qsTr("Ajustes del Reproductor")
    subtitle: qsTr("Configuración de motor libmpv, decodificación, audio y subtítulos")
    preferredWidth: 720

    property int currentTab: 0

    ColumnLayout {
        Layout.fillWidth: true
        Layout.maximumHeight: 520
        spacing: CachyTheme.spacingMedium

        CachySegmentedControl {
            Layout.fillWidth: true
            accentSelected: true
            model: [
                qsTr("Vídeo y GPU"),
                qsTr("Audio"),
                qsTr("Subtítulos"),
                qsTr("Caché y Red")
            ]
            currentIndex: root.currentTab
            onCurrentIndexChanged: root.currentTab = currentIndex
        }

        // =====================================================================
        // TAB 0: VÍDEO Y GPU
        // =====================================================================
        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 390
            visible: root.currentTab === 0
            contentWidth: availableWidth
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: CachyTheme.spacingSmall

                CachyFormCard {
                    Layout.fillWidth: true

                    CachyFormComboBoxDelegate {
                        label: qsTr("Perfil de Calidad (profile)")
                        description: qsTr("Perfil base para procesado de imagen y shaders")
                        model: ["high-quality", "fast", "default"]
                        currentIndex: model.indexOf(PlayerSettings.profile) !== -1 ? model.indexOf(PlayerSettings.profile) : 0
                        onActivated: (index) => PlayerSettings.profile = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("API de Gráficos (gpu-api)")
                        description: qsTr("Motor de renderizado por hardware")
                        model: ["vulkan", "opengl", "auto"]
                        currentIndex: model.indexOf(PlayerSettings.gpuApi) !== -1 ? model.indexOf(PlayerSettings.gpuApi) : 0
                        onActivated: (index) => PlayerSettings.gpuApi = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Contexto Gráfico (gpu-context)")
                        description: qsTr("Plataforma de visualización de bajo nivel")
                        model: ["waylandvk", "wayland", "auto"]
                        currentIndex: model.indexOf(PlayerSettings.gpuContext) !== -1 ? model.indexOf(PlayerSettings.gpuContext) : 0
                        onActivated: (index) => PlayerSettings.gpuContext = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Decodificación por Hardware (hwdec)")
                        description: qsTr("Decodificador asistido por GPU (VAAPI/Intel)")
                        model: ["auto", "auto-safe", "vaapi", "no"]
                        currentIndex: model.indexOf(PlayerSettings.hwdec) !== -1 ? model.indexOf(PlayerSettings.hwdec) : 0
                        onActivated: (index) => PlayerSettings.hwdec = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Escalado de Luma (scale)")
                        description: qsTr("Filtro para nitidez y escalado de luminancia")
                        model: ["ewa_lanczos", "spline36", "bicubic", "bilinear"]
                        currentIndex: model.indexOf(PlayerSettings.scale) !== -1 ? model.indexOf(PlayerSettings.scale) : 0
                        onActivated: (index) => PlayerSettings.scale = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Escalado de Croma (cscale)")
                        description: qsTr("Filtro para escalado de planos de color")
                        model: ["lanczos", "ewa_lanczos", "spline36", "bilinear"]
                        currentIndex: model.indexOf(PlayerSettings.cscale) !== -1 ? model.indexOf(PlayerSettings.cscale) : 0
                        onActivated: (index) => PlayerSettings.cscale = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Sincronización con Pantalla (video-sync)")
                        description: qsTr("Resamplea cuadros para cuadrar con los Hz del monitor")
                        model: ["display-resample", "audio", "display-vdrop"]
                        currentIndex: model.indexOf(PlayerSettings.videoSync) !== -1 ? model.indexOf(PlayerSettings.videoSync) : 0
                        onActivated: (index) => PlayerSettings.videoSync = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Interpolación de Cuadros (interpolation)")
                        description: qsTr("Suaviza el movimiento en pantallas con refresco no múltiplo")
                        checked: PlayerSettings.interpolation
                        onToggled: PlayerSettings.interpolation = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Filtro Antibanding (deband)")
                        description: qsTr("Reduce banding en degradados y cielos")
                        checked: PlayerSettings.deband
                        onToggled: PlayerSettings.deband = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Saltar Cabeceras / Intros Automáticamente")
                        description: qsTr("Salta la introducción al reproducir series si el archivo tiene marcas de capítulo")
                        checked: PlayerSettings.autoSkipIntro
                        onToggled: PlayerSettings.autoSkipIntro = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Recordar Posición al Salir (save-position-on-quit)")
                        description: qsTr("Guarda la marca de tiempo al cerrar el archivo")
                        checked: PlayerSettings.savePositionOnQuit
                        onToggled: PlayerSettings.savePositionOnQuit = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Reanudar Reproducción (resume-playback)")
                        description: qsTr("Retoma automáticamente desde la última posición guardada")
                        checked: PlayerSettings.resumePlayback
                        onToggled: PlayerSettings.resumePlayback = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Ocultar cursor automáticamente")
                        description: qsTr("Tiempo de inactividad del ratón antes de ocultar el puntero en reproducción")
                        model: [
                            qsTr("1 segundo"),
                            qsTr("2 segundos"),
                            qsTr("3 segundos"),
                            qsTr("5 segundos"),
                            qsTr("Nunca")
                        ]
                        property var timeoutValues: [1000, 2000, 3000, 5000, 0]
                        currentIndex: {
                            var idx = timeoutValues.indexOf(PlayerSettings.cursorHideTimeout)
                            return idx !== -1 ? idx : 1
                        }
                        onActivated: (index) => {
                            PlayerSettings.cursorHideTimeout = timeoutValues[index]
                        }
                    }
                }
            }
        }

        // =====================================================================
        // TAB 1: AUDIO
        // =====================================================================
        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 390
            visible: root.currentTab === 1
            contentWidth: availableWidth
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: CachyTheme.spacingSmall

                CachyFormCard {
                    Layout.fillWidth: true

                    CachyFormTextFieldDelegate {
                        label: qsTr("Idiomas Preferidos de Audio (alang)")
                        text: PlayerSettings.alang
                        onAccepted: PlayerSettings.alang = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Servidor de Salida (ao)")
                        description: qsTr("Motor de encaminamiento de audio")
                        model: ["pipewire", "pulse", "alsa", "jack"]
                        currentIndex: model.indexOf(PlayerSettings.ao) !== -1 ? model.indexOf(PlayerSettings.ao) : 0
                        onActivated: (index) => PlayerSettings.ao = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Volumen Máximo (volume-max)")
                        from: 100
                        to: 200
                        stepSize: 5
                        suffix: "%"
                        value: PlayerSettings.maxVolume
                        onValueModified: (val) => PlayerSettings.maxVolume = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Detección de Archivos de Audio (audio-file-auto)")
                        description: qsTr("Carga automática de pistas de audio externas")
                        model: ["fuzzy", "exact", "all", "no"]
                        currentIndex: model.indexOf(PlayerSettings.audioFileAuto) !== -1 ? model.indexOf(PlayerSettings.audioFileAuto) : 0
                        onActivated: (index) => PlayerSettings.audioFileAuto = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Visualización de Audio (audio-display)")
                        description: qsTr("Modo de presentación para archivos de sonido")
                        model: ["embedded-first", "no", "attachment"]
                        currentIndex: model.indexOf(PlayerSettings.audioDisplay) !== -1 ? model.indexOf(PlayerSettings.audioDisplay) : 0
                        onActivated: (index) => PlayerSettings.audioDisplay = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Reproducción Sin Pausas (gapless-audio)")
                        description: qsTr("Transición continua entre canciones consecutivas")
                        checked: PlayerSettings.gaplessAudio
                        onToggled: PlayerSettings.gaplessAudio = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Remezcla de Canales libavcodec (ad-lavc-downmix)")
                        description: qsTr("Permite remezclar canales surround a estéreo con libavcodec")
                        checked: PlayerSettings.adLavcDownmix
                        onToggled: PlayerSettings.adLavcDownmix = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Tamaño Filtro Resampleo (audio-resample-filter-size)")
                        from: 8
                        to: 64
                        stepSize: 8
                        value: PlayerSettings.resampleFilterSize
                        onValueModified: (val) => PlayerSettings.resampleFilterSize = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Desfase de Fase Resampleo (audio-resample-phase-shift)")
                        from: 0
                        to: 30
                        stepSize: 1
                        value: PlayerSettings.resamplePhaseShift
                        onValueModified: (val) => PlayerSettings.resamplePhaseShift = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Resampleo Lineal (audio-resample-linear)")
                        description: qsTr("Interpolación lineal de audio")
                        checked: PlayerSettings.resampleLinear
                        onToggled: PlayerSettings.resampleLinear = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Normalización de Audio (audio-normalize-downmix)")
                        description: qsTr("Evita saturación en remezcla estéreo de pistas 5.1/7.1")
                        checked: PlayerSettings.audioNormalize
                        onToggled: PlayerSettings.audioNormalize = checked
                    }
                }
            }
        }

        // =====================================================================
        // TAB 2: SUBTÍTULOS
        // =====================================================================
        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 390
            visible: root.currentTab === 2
            contentWidth: availableWidth
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: CachyTheme.spacingSmall

                CachyFormCard {
                    Layout.fillWidth: true

                    CachyFormTextFieldDelegate {
                        label: qsTr("Idiomas Preferidos de Subtítulos (slang)")
                        text: PlayerSettings.slang
                        onAccepted: PlayerSettings.slang = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Herramienta de Sincronización")
                        description: qsTr("Motor de resincronización automática de subtítulos")
                        model: ["ffsubsync", "alass"]
                        currentIndex: model.indexOf(PlayerSettings.syncTool) !== -1 ? model.indexOf(PlayerSettings.syncTool) : 0
                        onActivated: (index) => PlayerSettings.syncTool = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Carga Automática de Subtítulos (sub-auto)")
                        description: qsTr("Detección de archivos .srt/.vtt externos")
                        model: ["fuzzy", "exact", "all", "no"]
                        currentIndex: model.indexOf(PlayerSettings.subAuto) !== -1 ? model.indexOf(PlayerSettings.subAuto) : 0
                        onActivated: (index) => PlayerSettings.subAuto = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Preroll MKV Subtítulos (demuxer-mkv-subtitle-preroll)")
                        description: qsTr("Carga subtítulos previos al buscar en el archivo")
                        checked: PlayerSettings.subDemuxerMkvPreroll
                        onToggled: PlayerSettings.subDemuxerMkvPreroll = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Corrección de Tiempos (sub-fix-timing)")
                        description: qsTr("Ajusta pequeños solapamientos entre subtítulos consecutivos")
                        checked: PlayerSettings.subFixTiming
                        onToggled: PlayerSettings.subFixTiming = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Fuentes Embebidas (embeddedfonts)")
                        description: qsTr("Usa las fuentes integradas en el contenedor MKV")
                        checked: PlayerSettings.embeddedfonts
                        onToggled: PlayerSettings.embeddedfonts = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Escalar con Ventana (sub-scale-with-window)")
                        description: qsTr("Ajusta el tamaño del subtítulo según el tamaño de la ventana")
                        checked: PlayerSettings.subScaleWithWindow
                        onToggled: PlayerSettings.subScaleWithWindow = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Escalar ASS con Ventana (sub-ass-scale-with-window)")
                        description: qsTr("Ajusta subtítulos SSA/ASS al redimensionar la ventana")
                        checked: PlayerSettings.subAssScaleWithWindow
                        onToggled: PlayerSettings.subAssScaleWithWindow = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Proveedor de Fuentes (sub-font-provider)")
                        description: qsTr("Backend de búsqueda de fuentes tipográficas")
                        model: ["fontconfig", "auto", "none"]
                        currentIndex: model.indexOf(PlayerSettings.subFontProvider) !== -1 ? model.indexOf(PlayerSettings.subFontProvider) : 0
                        onActivated: (index) => PlayerSettings.subFontProvider = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Renderizar Estilos SSA/ASS (sub-ass)")
                        description: qsTr("Habilita renderizado de subtítulos avanzados SSA/ASS")
                        checked: PlayerSettings.subAss
                        onToggled: PlayerSettings.subAss = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Escala de Subtítulos (sub-scale, 50%-300%)")
                        from: 10
                        to: 500
                        stepSize: 1
                        suffix: "%"
                        value: Math.round(PlayerSettings.subScale * 100)
                        onValueModified: (val) => PlayerSettings.subScale = val / 100.0
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormTextFieldDelegate {
                        label: qsTr("Fuente (sub-font)")
                        text: PlayerSettings.subFont
                        onAccepted: PlayerSettings.subFont = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Tamaño de Fuente (sub-font-size)")
                        from: 20
                        to: 96
                        stepSize: 2
                        value: PlayerSettings.subFontSize
                        onValueModified: (val) => PlayerSettings.subFontSize = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Fuente en Negrita (sub-bold)")
                        checked: PlayerSettings.subBold
                        onToggled: PlayerSettings.subBold = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Fuente en Cursiva (sub-italic)")
                        checked: PlayerSettings.subItalic
                        onToggled: PlayerSettings.subItalic = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormTextFieldDelegate {
                        label: qsTr("Color de Texto (sub-color)")
                        text: PlayerSettings.subColor
                        onAccepted: PlayerSettings.subColor = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Estilo de Borde (sub-border-style)")
                        description: qsTr("Estilo visual del contorno de subtítulos")
                        model: ["outline-and-shadow", "opaque-box", "background-box"]
                        currentIndex: model.indexOf(PlayerSettings.subBorderStyle) !== -1 ? model.indexOf(PlayerSettings.subBorderStyle) : 0
                        onActivated: (index) => PlayerSettings.subBorderStyle = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormTextFieldDelegate {
                        label: qsTr("Color de Borde (sub-border-color)")
                        text: PlayerSettings.subBorderColor
                        onAccepted: PlayerSettings.subBorderColor = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormTextFieldDelegate {
                        label: qsTr("Color de Sombra (sub-shadow-color)")
                        text: PlayerSettings.subShadowColor
                        onAccepted: PlayerSettings.subShadowColor = text
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Margen Vertical (sub-margin-y)")
                        from: 10
                        to: 140
                        stepSize: 2
                        value: PlayerSettings.subMarginY
                        onValueModified: (val) => PlayerSettings.subMarginY = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormSpinBoxDelegate {
                        label: qsTr("Posición Vertical en Pantalla (sub-pos, 0=arriba, 100=abajo)")
                        from: 0
                        to: 150
                        stepSize: 1
                        value: PlayerSettings.subPos
                        onValueModified: (val) => PlayerSettings.subPos = val
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormComboBoxDelegate {
                        label: qsTr("Sobreescritura de Estilos ASS (sub-ass-override)")
                        description: qsTr("Permite forzar estilo propio sobre subtítulos avanzados")
                        model: ["scale", "yes", "force", "no"]
                        currentIndex: model.indexOf(PlayerSettings.subAssOverride) !== -1 ? model.indexOf(PlayerSettings.subAssOverride) : 0
                        onActivated: (index) => PlayerSettings.subAssOverride = model[index]
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Mezclar Subtítulos Antes de Escalar (blend-subtitles)")
                        description: qsTr("Integra los subtítulos con el vídeo antes de la interpolación")
                        checked: PlayerSettings.blendSubtitles
                        onToggled: PlayerSettings.blendSubtitles = checked
                    }
                }
            }
        }

        // =====================================================================
        // TAB 3: CACHÉ Y RED
        // =====================================================================
        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 390
            visible: root.currentTab === 3
            contentWidth: availableWidth
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: CachyTheme.spacingSmall

                CachyFormCard {
                    Layout.fillWidth: true

                    CachyFormCheckDelegate {
                        label: qsTr("Caché en Memoria (cache)")
                        description: qsTr("Usa búfer en memoria para reproducción fluida local y de red")
                        checked: PlayerSettings.cache
                        onToggled: PlayerSettings.cache = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Pausar al Vaciar Caché (cache-pause)")
                        description: qsTr("Pausa automáticamente el reproductor si el búfer se agota")
                        checked: PlayerSettings.cachePause
                        onToggled: PlayerSettings.cachePause = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Precarga de Lista (prefetch-playlist)")
                        description: qsTr("Precarga el siguiente archivo de la lista de reproducción")
                        checked: PlayerSettings.prefetchPlaylist
                        onToggled: PlayerSettings.prefetchPlaylist = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormCheckDelegate {
                        label: qsTr("Carga Automática de Episodios (autoload)")
                        description: qsTr("Encola archivos siguientes encontrados en la misma carpeta")
                        checked: PlayerSettings.autoLoadNext
                        onToggled: PlayerSettings.autoLoadNext = checked
                    }

                    CachyFormDelegateSeparator {}

                    CachyFormTextFieldDelegate {
                        label: qsTr("Origen de Cookies (ytdl-raw-options)")
                        text: PlayerSettings.ytdlCookies
                        onAccepted: PlayerSettings.ytdlCookies = text
                    }
                }
            }
        }

        // =====================================================================
        // FOOTER ACTIONS
        // =====================================================================
        RowLayout {
            Layout.fillWidth: true
            spacing: CachyTheme.spacingSmall

            CachyButton {
                text: qsTr("Restablecer Valores")
                onClicked: PlayerSettings.resetToDefaults()
            }

            Item { Layout.fillWidth: true }

            CachyButton {
                text: qsTr("Aceptar")
                primary: true
                onClicked: root.close()
            }
        }
    }
}
