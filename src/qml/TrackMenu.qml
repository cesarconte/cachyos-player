import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyPopover {
    id: root
    preferredWidth: 460
    maxHeight: 520

    property var trackList: mpvController.trackList
    property string trackType: "sub" // "sub" or "audio"
    property string currentVideoPath: ""
    signal trackSelected(string type, int id)

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 12

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            CachyHeading {
                level: 3
                text: root.trackType === "sub" ? qsTr("Subtítulos") : qsTr("Pistas de Audio")
                Layout.fillWidth: true
            }

            // Subtitle Actions (Sync & Download)
            RowLayout {
                visible: root.trackType === "sub"
                spacing: 6

                CachyButton {
                    text: SubSync.isSyncing ? qsTr("Procesando...") : qsTr("Sincronizar (%1)").arg(SubSync.tool)
                    iconName: "system-run"
                    enabled: !SubSync.isSyncing
                    onClicked: {
                        var selectedSub = root.trackList.find(t => t.type === "sub" && t.selected)
                        var subFile = ""
                        if (selectedSub) {
                            subFile = selectedSub["external-filename"] || selectedSub.externalFilename || ""
                        }
                        SubSync.syncSubtitle(root.currentVideoPath, subFile)
                    }
                }

                CachyButton {
                    iconName: "download"
                    text: SubSync.isSyncing ? qsTr("Buscando...") : qsTr("Descargar")
                    enabled: !SubSync.isSyncing
                    onClicked: {
                        SubSync.downloadSubtitles(root.currentVideoPath, "es")
                    }
                }
            }

            // EasyEffects Profile Selector (Only shown for Audio)
            RowLayout {
                visible: root.trackType === "audio" && EasyEffects.isAvailable
                spacing: 6

                CachyLabel {
                    text: qsTr("EasyEffects:")
                    font.pixelSize: 11
                    color: CachyPalette.textSecondary
                }

                CachyComboBox {
                    id: eeCombo
                    Layout.preferredWidth: 160
                    model: EasyEffects.presets
                    currentIndex: Math.max(0, EasyEffects.presets.indexOf(EasyEffects.currentPreset))
                    onActivated: (index) => {
                        var p = EasyEffects.presets[index]
                        EasyEffects.loadPreset(p)
                    }
                }
            }
        }

        CachyLabel {
            visible: SubSync.statusMessage.length > 0
            text: SubSync.statusMessage
            font.pixelSize: 11
            color: CachyTheme.accentColor
            Layout.fillWidth: true
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
        }

        CachyDivider {
            Layout.fillWidth: true
        }

        ListView {
            id: trackListView
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(220, Math.max(100, count * 52))
            clip: true
            model: root.trackList.filter(t => t.type === root.trackType)

            delegate: CachyListItem {
                width: trackListView.width
                title: {
                    var name = modelData.title || modelData.lang || (root.trackType + " #" + modelData.id);
                    if (modelData.external) name += " " + qsTr("[externo]");
                    return name;
                }
                subtitle: {
                    var parts = [];
                    if (modelData.codec) parts.push(modelData.codec);
                    if (modelData["audio-channels"]) parts.push(modelData["audio-channels"] + "ch");
                    if (modelData["audio-samplerate"]) parts.push((modelData["audio-samplerate"]/1000) + "kHz");
                    return parts.join(" • ");
                }
                statusText: modelData.selected ? "✓" : ""
                selected: modelData.selected

                onClicked: {
                    root.trackSelected(root.trackType, modelData.id)
                    root.close()
                }
            }

            CachyPlaceholderMessage {
                anchors.centerIn: parent
                visible: trackListView.count === 0
                text: root.trackType === "sub" ? qsTr("No hay subtítulos disponibles") : qsTr("No hay pistas de audio")
                iconName: root.trackType === "sub" ? "media-view-subtitles" : "audio-speakers"
            }
        }

        CachyDivider {
            Layout.fillWidth: true
            visible: root.trackType === "sub"
        }

        // Quick Subtitle Controls (Size, Position & Delay)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8
            visible: root.trackType === "sub"

            CachyLabel {
                text: qsTr("Ajustes Rápidos de Subtítulos")
                font.pixelSize: 11
                font.weight: Font.DemiBold
                color: CachyPalette.textSecondary
            }

            // Row 1: Tamaño y Posición vertical
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                // Tamaño (sub-scale)
                RowLayout {
                    spacing: 4
                    CachyLabel {
                        text: qsTr("Tamaño:")
                        font.pixelSize: 11
                        color: CachyPalette.textSecondary
                    }

                    CachyButton {
                        iconName: "format-font-size-less"
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 28
                        onClicked: {
                            var nextVal = Math.max(0.2, Math.round((mpvController.subScale - 0.05) * 100) / 100)
                            mpvController.subScale = nextVal
                            PlayerSettings.subScale = nextVal
                        }
                    }

                    CachyLabel {
                        text: Math.round(mpvController.subScale * 100) + "%"
                        font.pixelSize: 11
                        font.weight: Font.Medium
                        Layout.preferredWidth: 40
                        horizontalAlignment: Text.AlignHCenter
                    }

                    CachyButton {
                        iconName: "format-font-size-more"
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 28
                        onClicked: {
                            var nextVal = Math.min(3.0, Math.round((mpvController.subScale + 0.05) * 100) / 100)
                            mpvController.subScale = nextVal
                            PlayerSettings.subScale = nextVal
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                // Posición vertical (sub-pos)
                RowLayout {
                    spacing: 4
                    CachyLabel {
                        text: qsTr("Posición:")
                        font.pixelSize: 11
                        color: CachyPalette.textSecondary
                    }

                    CachyButton {
                        iconName: "go-up"
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 28
                        onClicked: {
                            var nextPos = Math.max(0, mpvController.subPos - 1)
                            mpvController.subPos = nextPos
                            PlayerSettings.subPos = nextPos
                        }
                    }

                    CachyLabel {
                        text: mpvController.subPos === 100 ? qsTr("Abajo") : (mpvController.subPos < 50 ? qsTr("Arriba") : (mpvController.subPos + "%"))
                        font.pixelSize: 11
                        font.weight: Font.Medium
                        Layout.preferredWidth: 44
                        horizontalAlignment: Text.AlignHCenter
                    }

                    CachyButton {
                        iconName: "go-down"
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 28
                        onClicked: {
                            var nextPos = Math.min(150, mpvController.subPos + 1)
                            mpvController.subPos = nextPos
                            PlayerSettings.subPos = nextPos
                        }
                    }
                }
            }

            // Row 2: Sincronización / Desfase temporal (sub-delay)
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                CachyLabel {
                    text: qsTr("Sincronización:")
                    font.pixelSize: 11
                    color: CachyPalette.textSecondary
                    Layout.fillWidth: true
                }

                CachyButton {
                    text: "−0.5s"
                    Layout.preferredHeight: 28
                    onClicked: {
                        var nextDel = Math.round((mpvController.subDelay - 0.5) * 10) / 10
                        mpvController.subDelay = nextDel
                        PlayerSettings.subDelay = nextDel
                    }
                }

                CachyButton {
                    text: "−0.1s"
                    Layout.preferredHeight: 28
                    onClicked: {
                        var nextDel = Math.round((mpvController.subDelay - 0.1) * 10) / 10
                        mpvController.subDelay = nextDel
                        PlayerSettings.subDelay = nextDel
                    }
                }

                CachyLabel {
                    text: (mpvController.subDelay >= 0 ? "+" : "") + mpvController.subDelay.toFixed(1) + "s"
                    font.pixelSize: 11
                    font.weight: Font.Bold
                    color: mpvController.subDelay === 0 ? CachyPalette.textSecondary : CachyTheme.accentColor
                    Layout.preferredWidth: 44
                    horizontalAlignment: Text.AlignHCenter
                }

                CachyButton {
                    text: "+0.1s"
                    Layout.preferredHeight: 28
                    onClicked: {
                        var nextDel = Math.round((mpvController.subDelay + 0.1) * 10) / 10
                        mpvController.subDelay = nextDel
                        PlayerSettings.subDelay = nextDel
                    }
                }

                CachyButton {
                    text: "+0.5s"
                    Layout.preferredHeight: 28
                    onClicked: {
                        var nextDel = Math.round((mpvController.subDelay + 0.5) * 10) / 10
                        mpvController.subDelay = nextDel
                        PlayerSettings.subDelay = nextDel
                    }
                }

                CachyButton {
                    text: qsTr("0s")
                    Layout.preferredHeight: 28
                    visible: mpvController.subDelay !== 0
                    onClicked: {
                        mpvController.subDelay = 0.0
                        PlayerSettings.subDelay = 0.0
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            CachyButton {
                text: root.trackType === "sub" ? qsTr("Desactivar Subtítulos") : qsTr("Sin Audio")
                Layout.fillWidth: true
                onClicked: {
                    root.trackSelected(root.trackType, -1)
                    root.close()
                }
            }

            CachyButton {
                text: qsTr("Cargar archivo...")
                iconName: "document-open"
                visible: root.trackType === "sub"
                onClicked: subDialogHelper.openSingleFile(qsTr("Seleccionar archivo de subtítulos"), qsTr("Subtítulos (*.srt *.ass *.ssa *.vtt *.sub)"))
            }
        }
    }

    NativeDialogHelper {
        id: subDialogHelper
        onFileSelected: (filePath) => {
            mpvController.command(["sub-add", filePath, "select"])
            root.close()
        }
    }
}
