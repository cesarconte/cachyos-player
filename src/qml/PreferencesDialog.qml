import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyDialog {
    id: root
    title: qsTr("Ajustes del Reproductor")
    preferredWidth: 540

    CachyScrollablePage {
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            width: parent.width
            spacing: 16

            // Section 1: Video Engine & Hardware
            CachyFormHeader {
                title: qsTr("Vídeo y Renderizado")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormComboBoxDelegate {
                    label: qsTr("API de Gráficos")
                    description: qsTr("Motor de renderizado de ultra baja latencia")
                    model: ["vulkan", "opengl", "auto"]
                    currentIndex: model.indexOf(PlayerSettings.gpuApi) !== -1 ? model.indexOf(PlayerSettings.gpuApi) : 0
                    onActivated: (index) => PlayerSettings.gpuApi = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Aceleración por Hardware")
                    description: qsTr("Decodificación asistida por GPU (Intel UHD 770 VAAPI)")
                    model: ["auto-safe", "vaapi", "auto", "no"]
                    currentIndex: model.indexOf(PlayerSettings.hwdec) !== -1 ? model.indexOf(PlayerSettings.hwdec) : 0
                    onActivated: (index) => PlayerSettings.hwdec = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormComboBoxDelegate {
                    label: qsTr("Algoritmo de Reescalado")
                    description: qsTr("Filtro de nitidez y escalado de texturas")
                    model: ["ewa_lanczos", "spline36", "bicubic", "bilinear"]
                    currentIndex: model.indexOf(PlayerSettings.scale) !== -1 ? model.indexOf(PlayerSettings.scale) : 0
                    onActivated: (index) => PlayerSettings.scale = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Interpolación de Fotogramas")
                    description: qsTr("Suavizado de movimiento a los Hz del monitor")
                    checked: PlayerSettings.interpolation
                    onToggled: PlayerSettings.interpolation = checked
                }
            }

            // Section 2: Audio & PipeWire
            CachyFormHeader {
                title: qsTr("Audio y Sonido")
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

                CachyFormComboBoxDelegate {
                    label: qsTr("Calidad de Resampling")
                    description: qsTr("Filtro sinc SoXR de máxima precisión")
                    model: ["high", "normal"]
                    currentIndex: model.indexOf(PlayerSettings.resampleQuality) !== -1 ? model.indexOf(PlayerSettings.resampleQuality) : 0
                    onActivated: (index) => PlayerSettings.resampleQuality = model[index]
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Normalización Inteligente")
                    description: qsTr("Equilibra el volumen dinámico en diálogos")
                    checked: PlayerSettings.audioNormalize
                    onToggled: PlayerSettings.audioNormalize = checked
                }
            }

            // Section 3: Subtitles & Auto-utilities
            CachyFormHeader {
                title: qsTr("Subtítulos y Automatización")
            }

            CachyFormCard {
                Layout.fillWidth: true

                CachyFormTextFieldDelegate {
                    label: qsTr("Idioma Preferido de Subtítulos")
                    text: PlayerSettings.subLanguage
                    onAccepted: PlayerSettings.subLanguage = text
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Sincronización Automática (autosubsync)")
                    description: qsTr("Ajustar retraso de subtítulos con el audio automáticamente")
                    checked: PlayerSettings.autoSubSync
                    onToggled: PlayerSettings.autoSubSync = checked
                }

                CachyFormDelegateSeparator {}

                CachyFormCheckDelegate {
                    label: qsTr("Carga Automática de Episodios (autoload)")
                    description: qsTr("Encolar archivos siguientes encontrados en la misma carpeta")
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
