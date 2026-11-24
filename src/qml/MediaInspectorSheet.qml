/*
 * SPDX-FileCopyrightText: 2026 CachyOS UI contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

ModalSheet {
    id: root
    title: qsTr("Información del archivo")
    subtitle: qsTr("Inspector técnico de códecs, pistas y rendimiento de reproducción")
    preferredWidth: 640

    property var info: ({})

    function refresh() {
        if (typeof mpvController !== "undefined" && mpvController) {
            info = mpvController.getMediaInfo()
        }
    }

    onOpened: refresh()

    function formatBytes(bytes) {
        if (!bytes || bytes <= 0) return qsTr("Desconocido")
        var sizes = ['B', 'KB', 'MB', 'GB', 'TB']
        var i = Math.floor(Math.log(bytes) / Math.log(1024))
        return (bytes / Math.pow(1024, i)).toFixed(2) + ' ' + sizes[i]
    }

    function formatBitrate(bps) {
        if (!bps || bps <= 0) return qsTr("Desconocido")
        return Math.round(bps / 1000) + " kbps"
    }

    component InfoRow : Item {
        id: infoRow
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        implicitHeight: Math.max(38, Math.ceil(rowContent.implicitHeight) + 12)

        RowLayout {
            id: rowContent
            anchors.fill: parent
            anchors.leftMargin: CachyTheme.spacingMedium
            anchors.rightMargin: CachyTheme.spacingMedium
            spacing: CachyTheme.spacingSmall

            CachyLabel {
                text: infoRow.label
                font.weight: Font.Medium
                color: CachyPalette.text
                Layout.preferredWidth: 200
                elide: Text.ElideRight
            }

            CachyLabel {
                text: infoRow.value
                color: CachyPalette.textSecondary
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                elide: Text.ElideMiddle
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.maximumHeight: 500
        spacing: CachyTheme.spacingMedium

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 400
            contentWidth: availableWidth
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: CachyTheme.spacingMedium

                // 1. Archivo y Contenedor
                CachyFormCard {
                    Layout.fillWidth: true

                    InfoRow {
                        label: qsTr("Título")
                        value: root.info.mediaTitle || qsTr("Sin título")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Ruta")
                        value: root.info.currentPath || qsTr("Desconocido")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Tamaño del archivo")
                        value: root.formatBytes(root.info.fileSize)
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Formato contenedor")
                        value: (root.info.fileFormat || qsTr("Desconocido")).toUpperCase()
                    }
                }

                // 2. Transmisión de Vídeo
                CachyFormCard {
                    Layout.fillWidth: true

                    InfoRow {
                        label: qsTr("Códec de vídeo")
                        value: (root.info.videoCodec || root.info.videoCodecName || qsTr("Ninguno")).toUpperCase()
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Resolución real")
                        value: root.info.videoWidth > 0 ? (root.info.videoWidth + " × " + root.info.videoHeight) : qsTr("N/A")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Resolución de despliegue")
                        value: root.info.dwidth > 0 ? (root.info.dwidth + " × " + root.info.dheight) : qsTr("N/A")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Fotogramas por segundo (FPS)")
                        value: root.info.fps > 0 ? (root.info.fps.toFixed(3) + " fps") : qsTr("Desconocido")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Tasa de bits de vídeo")
                        value: root.formatBitrate(root.info.videoBitrate)
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Decodificación por hardware (hwdec)")
                        value: root.info.hwdecCurrent && root.info.hwdecCurrent.length > 0 ? root.info.hwdecCurrent : qsTr("Software")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Cuadros descartados (drop)")
                        value: String(root.info.frameDropCount || 0)
                    }
                }

                // 3. Transmisión de Audio
                CachyFormCard {
                    Layout.fillWidth: true

                    InfoRow {
                        label: qsTr("Códec de audio")
                        value: (root.info.audioCodec || root.info.audioCodecName || qsTr("Ninguno")).toUpperCase()
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Canales")
                        value: root.info.audioChannels ? String(root.info.audioChannels) : qsTr("Desconocido")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Frecuencia de muestreo")
                        value: root.info.audioSampleRate > 0 ? (root.info.audioSampleRate + " Hz") : qsTr("Desconocido")
                    }

                    CachyFormDelegateSeparator {}

                    InfoRow {
                        label: qsTr("Tasa de bits de audio")
                        value: root.formatBitrate(root.info.audioBitrate)
                    }
                }
            }
        }
    }
}
