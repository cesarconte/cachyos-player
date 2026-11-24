import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CachyOS.UI
import org.cachyos.player

CachyPopover {
    id: root
    preferredWidth: 460
    maxHeight: 520

    property var trackList: []
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
                        if (selectedSub && selectedSub.externalFilename) {
                            SubSync.syncSubtitle(root.currentVideoPath, selectedSub.externalFilename)
                        } else {
                            SubSync.syncSubtitle(root.currentVideoPath, "")
                        }
                    }
                }

                CachyButton {
                    iconName: "download"
                    text: qsTr("Descargar")
                    enabled: !SubSync.isSyncing
                    onClicked: {
                        SubSync.downloadSubtitles(root.currentVideoPath, "es")
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
        }

        CachyDivider {
            Layout.fillWidth: true
        }

        ListView {
            id: trackListView
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(320, Math.max(120, count * 52))
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
                onClicked: subFileDialog.open()
            }
        }
    }

    FileDialog {
        id: subFileDialog
        title: qsTr("Seleccionar archivo de subtítulos")
        nameFilters: [qsTr("Subtítulos (*.srt *.ass *.ssa *.vtt *.sub)"), qsTr("Todos los archivos (*)")]
        onAccepted: {
            mpvController.command(["sub-add", subFileDialog.selectedFile.toString().replace("file://", ""), "select"])
            root.close()
        }
    }
}
