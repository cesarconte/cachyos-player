import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyPopover {
    id: root
    preferredWidth: 340
    maxHeight: 440

    property var trackList: []
    property string trackType: "sub" // "sub" or "audio"
    property string currentVideoPath: ""
    signal trackSelected(string type, int id)

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        anchors.margins: 14
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            CachyHeading {
                level: 3
                text: root.trackType === "sub" ? qsTr("Subtítulos") : qsTr("Pistas de Audio")
                Layout.fillWidth: true
            }

            // Sync button (for subtitles)
            CachyButton {
                visible: root.trackType === "sub"
                text: SubSync.isSyncing ? qsTr("Sincronizando...") : qsTr("Sincronizar")
                iconName: "system-run"
                enabled: !SubSync.isSyncing
                onClicked: {
                    var selectedSub = root.trackList.find(t => t.type === "sub" && t.selected)
                    if (selectedSub && selectedSub.externalFilename) {
                        SubSync.syncSubtitle(root.currentVideoPath, selectedSub.externalFilename)
                    } else {
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
            Layout.fillHeight: true
            clip: true
            model: root.trackList.filter(t => t.type === root.trackType)

            delegate: CachyListItem {
                width: ListView.view.width
                title: modelData.title || modelData.lang || (root.trackType + " #" + modelData.id)
                subtitle: modelData.codec ? qsTr("Códec: %1").arg(modelData.codec) : ""
                statusText: modelData.selected ? "✓" : ""
                selected: modelData.selected

                onClicked: {
                    root.trackSelected(root.trackType, modelData.id)
                    root.close()
                }
            }
        }

        CachyButton {
            text: root.trackType === "sub" ? qsTr("Desactivar Subtítulos") : qsTr("Sin Audio")
            Layout.fillWidth: true
            onClicked: {
                root.trackSelected(root.trackType, -1)
                root.close()
            }
        }
    }
}
