import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI

CachyPopover {
    id: root
    preferredWidth: 320
    maxHeight: 380

    property var trackList: []
    property string trackType: "sub" // "sub" or "audio"
    signal trackSelected(string type, int id)

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        anchors.margins: 12
        spacing: 8

        CachyHeading {
            level: 3
            text: root.trackType === "sub" ? qsTr("Subtítulos") : qsTr("Pistas de Audio")
            Layout.fillWidth: true
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
