import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyDialog {
    id: root
    title: qsTr("Ajustes del Reproductor")
    preferredWidth: 680

    CachyScrollablePage {
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            width: parent.width
            spacing: 16

            // =========================================================
            // 1. GENERAL & VIDEO
            // =========================================================
            CachyFormHeader {
                title: qsTr("1. General y Motor de Vídeo (Intel UHD 770 / Vulkan)")
            }

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
                    label: qsTr("Interpolación de Fotogramas (interpolation)")
                    description: qsTr("Elimina microtirones (judder) al reproducir 24 fps a 75 Hz")
                    checked: PlayerSettings.interpolation
                    onToggled: PlayerSettings.interpolation = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Filtro Debanding (deband)")
                    description: qsTr("Elimina bandas en degradados oscuros o escenas comprimidas")
                    checked: PlayerSettings.deband
                    onToggled: PlayerSettings.deband = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Guardar Posición al Salir (save-position-on-quit)")
                    description: qsTr("Recuerda el punto exacto de visualización de cada archivo")
                    checked: PlayerSettings.savePositionOnQuit
                    onToggled: PlayerSettings.savePositionOnQuit = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Reanudar Reproducción (resume-playback)")
                    description: qsTr("Reanuda automáticamente streams y contenido online")
                    checked: PlayerSettings.resumePlayback
                    onToggled: PlayerSettings.resumePlayback = checked
                }
            }

            // =========================================================
            // 2. AUDIO & PIPEWIRE
            // =========================================================
            CachyFormHeader {
                title: qsTr("2. Audio y Sonido (PipeWire Bit-Perfect)")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("Servidor de Audio (ao)")
                    description: qsTr("Salida directa a PipeWire o subsistemas de audio")
                    model: ["pipewire", "pulse", "alsa"]
                    currentIndex: model.indexOf(PlayerSettings.ao) !== -1 ? model.indexOf(PlayerSettings.ao) : 0
                    onActivated: (index) => PlayerSettings.ao = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Reproducción Sin Pausas (gapless-audio)")
                    description: qsTr("Evita silencios o clicks entre pistas en álbumes de música")
                    checked: PlayerSettings.gaplessAudio
                    onToggled: PlayerSettings.gaplessAudio = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Límite Máximo de Volumen (%) (volume-max)")
                    from: 100
                    to: 150
                    value: PlayerSettings.maxVolume
                    onValueModified: PlayerSettings.maxVolume = value
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Filtro de Resampling (audio-resample-filter-size)")
                    from: 8
                    to: 64
                    value: PlayerSettings.resampleFilterSize
                    onValueModified: PlayerSettings.resampleFilterSize = value
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Frecuencia de Corte (%) (audio-resample-cutoff)")
                    from: 80
                    to: 100
                    value: Math.round(PlayerSettings.resampleCutoff * 100)
                    onValueModified: PlayerSettings.resampleCutoff = value / 100.0
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Desplazamiento de Fase (audio-resample-phase-shift)")
                    from: 0
                    to: 30
                    value: PlayerSettings.resamplePhaseShift
                    onValueModified: PlayerSettings.resamplePhaseShift = value
                }
            }

            // =========================================================
            // 3. SUBTITLES & STYLING
            // =========================================================
            CachyFormHeader {
                title: qsTr("3. Subtítulos, Tipografía y Sincronización")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("Herramienta de Sincronización")
                    description: qsTr("Motor para sincronizar subtítulos desfasados")
                    model: ["ffsubsync", "alass"]
                    currentIndex: model.indexOf(PlayerSettings.syncTool) !== -1 ? model.indexOf(PlayerSettings.syncTool) : 0
                    onActivated: (index) => {
                        PlayerSettings.syncTool = model[index];
                        SubSync.tool = model[index];
                    }
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Prioridad de Idiomas de Subtítulos (slang)")
                    text: PlayerSettings.slang
                    onAccepted: PlayerSettings.slang = text
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Prioridad de Idiomas de Audio (alang)")
                    text: PlayerSettings.alang
                    onAccepted: PlayerSettings.alang = text
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Familia Tipográfica SRT (sub-font)")
                    text: PlayerSettings.subFont
                    onAccepted: PlayerSettings.subFont = text
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Tamaño de Fuente (sub-font-size)")
                    from: 18
                    to: 96
                    value: PlayerSettings.subFontSize
                    onValueModified: PlayerSettings.subFontSize = value
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Color Primario del Texto (sub-color)")
                    text: PlayerSettings.subColor
                    onAccepted: PlayerSettings.subColor = text
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Color del Borde / Sombra (sub-border-color)")
                    text: PlayerSettings.subBorderColor
                    onAccepted: PlayerSettings.subBorderColor = text
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Margen Vertical Inferior (sub-margin-y)")
                    from: 0
                    to: 150
                    value: PlayerSettings.subMarginY
                    onValueModified: PlayerSettings.subMarginY = value
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Modo de Anulación ASS (sub-ass-override)")
                    description: qsTr("scale: respeta estilos ASS; force: impone estilo SRT")
                    model: ["scale", "force", "no", "yes"]
                    currentIndex: model.indexOf(PlayerSettings.subAssOverride) !== -1 ? model.indexOf(PlayerSettings.subAssOverride) : 0
                    onActivated: (index) => PlayerSettings.subAssOverride = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Mezclar Subtítulos con Vídeo (blend-subtitles)")
                    description: qsTr("Renderiza subtítulos antes de interpolar fotogramas")
                    checked: PlayerSettings.blendSubtitles
                    onToggled: PlayerSettings.blendSubtitles = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Carga Automática de Episodios (autoload)")
                    description: qsTr("Encola archivos siguientes encontrados en la misma carpeta")
                    checked: PlayerSettings.autoLoadNext
                    onToggled: PlayerSettings.autoLoadNext = checked
                }
            }

            // =========================================================
            // 4. STREAMING & YT-DLP
            // =========================================================
            CachyFormHeader {
                title: qsTr("4. Streaming y Opciones yt-dlp")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormTextFieldDelegate {
                    label: qsTr("Origen de Cookies (ytdl-raw-options)")
                    text: PlayerSettings.ytdlCookies
                    onAccepted: PlayerSettings.ytdlCookies = text
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }

                CachyButton {
                    text: qsTr("Restablecer Valores")
                    onClicked: PlayerSettings.resetToDefaults()
                }

                CachyButton {
                    text: qsTr("Aceptar")
                    primary: true
                    onClicked: root.close()
                }
            }
        }
    }
}
