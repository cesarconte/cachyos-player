import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyDialog {
    id: root
    title: qsTr("Ajustes del Reproductor")
    preferredWidth: 640

    CachyScrollablePage {
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            width: parent.width
            spacing: 16

            // Section 1: Video Engine & Hardware (from mpv.conf)
            CachyFormHeader {
                title: qsTr("Vídeo y Renderizado (Intel UHD 770 / Vulkan)")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("API de Gráficos")
                    description: qsTr("Vulkan nativo en Wayland (gpu-context=waylandvk)")
                    model: ["vulkan", "opengl", "auto"]
                    currentIndex: model.indexOf(PlayerSettings.gpuApi) !== -1 ? model.indexOf(PlayerSettings.gpuApi) : 0
                    onActivated: (index) => PlayerSettings.gpuApi = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Aceleración por Hardware")
                    description: qsTr("Decodificación asistida por GPU (VAAPI/Intel)")
                    model: ["auto", "auto-safe", "vaapi", "no"]
                    currentIndex: model.indexOf(PlayerSettings.hwdec) !== -1 ? model.indexOf(PlayerSettings.hwdec) : 0
                    onActivated: (index) => PlayerSettings.hwdec = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Algoritmo de Reescalado (Luma)")
                    description: qsTr("Filtro de nitidez e interpolación de texturas")
                    model: ["ewa_lanczos", "spline36", "bicubic", "bilinear"]
                    currentIndex: model.indexOf(PlayerSettings.scale) !== -1 ? model.indexOf(PlayerSettings.scale) : 0
                    onActivated: (index) => PlayerSettings.scale = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Algoritmo de Croma (Chroma scale)")
                    description: qsTr("Filtro para planos de color")
                    model: ["lanczos", "ewa_lanczos", "spline36", "bilinear"]
                    currentIndex: model.indexOf(PlayerSettings.cscale) !== -1 ? model.indexOf(PlayerSettings.cscale) : 0
                    onActivated: (index) => PlayerSettings.cscale = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Interpolación de Fotogramas (video-sync)")
                    description: qsTr("Suaviza el movimiento a los 75 Hz de la pantalla")
                    checked: PlayerSettings.interpolation
                    onToggled: PlayerSettings.interpolation = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Filtro Debanding (deband)")
                    description: qsTr("Reduce artefactos y bandas en degradados de color")
                    checked: PlayerSettings.deband
                    onToggled: PlayerSettings.deband = checked
                }
            }

            // Section 2: Audio & PipeWire
            CachyFormHeader {
                title: qsTr("Audio y Sonido (PipeWire Bit-Perfect)")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("Servidor de Audio")
                    description: qsTr("Salida directa sin remuestreo innecesario")
                    model: ["pipewire", "pulse", "alsa"]
                    currentIndex: model.indexOf(PlayerSettings.ao) !== -1 ? model.indexOf(PlayerSettings.ao) : 0
                    onActivated: (index) => PlayerSettings.ao = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Reproducción Sin Pausas (gapless-audio)")
                    description: qsTr("Transición suave entre pistas de audio continuas")
                    checked: PlayerSettings.gaplessAudio
                    onToggled: PlayerSettings.gaplessAudio = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Límite Máximo de Volumen (%)")
                    from: 100
                    to: 150
                    value: PlayerSettings.maxVolume
                    onValueModified: PlayerSettings.maxVolume = value
                }
            }

            // Section 3: Subtitles & Synchronization
            CachyFormHeader {
                title: qsTr("Subtítulos y Motor de Sincronización")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("Herramienta de Sincronización")
                    description: qsTr("Motor preferido para alinear audio y subtítulos")
                    model: ["ffsubsync", "alass"]
                    currentIndex: model.indexOf(PlayerSettings.syncTool) !== -1 ? model.indexOf(PlayerSettings.syncTool) : 0
                    onActivated: (index) => {
                        PlayerSettings.syncTool = model[index];
                        SubSync.tool = model[index];
                    }
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Prioridad de Idioma de Subtítulos (slang)")
                    text: PlayerSettings.subLanguage
                    onAccepted: PlayerSettings.subLanguage = text
                }

                CachyFormDelegateSeparator {}

                CachyFormTextFieldDelegate {
                    label: qsTr("Tipografía de Subtítulos SRT (sub-font)")
                    text: PlayerSettings.subFont
                    onAccepted: PlayerSettings.subFont = text
                }

                CachyFormDelegateSeparator {}

                CachyFormSpinBoxDelegate {
                    label: qsTr("Tamaño de Fuente (sub-font-size)")
                    from: 20
                    to: 80
                    value: PlayerSettings.subFontSize
                    onValueModified: PlayerSettings.subFontSize = value
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Mezcla de Subtítulos con Vídeo (blend-subtitles)")
                    description: qsTr("Renderiza subtítulos antes del reescalado para máxima nitidez")
                    checked: PlayerSettings.blendSubtitles
                    onToggled: PlayerSettings.blendSubtitles = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Carga Automática de Episodios (autoload)")
                    description: qsTr("Encola automáticamente archivos contiguos de la misma serie")
                    checked: PlayerSettings.autoLoadNext
                    onToggled: PlayerSettings.autoLoadNext = checked
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
