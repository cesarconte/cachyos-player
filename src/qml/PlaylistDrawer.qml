import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

CachyDrawer {
    id: root
    edge: Qt.RightEdge
    width: Math.min(360, parent.width * 0.8)

    property var playlist: []
    property int currentIndex: -1

    signal itemSelected(int index)
    signal itemRemoved(int index)
    signal addFileRequested()
    signal addFolderRequested()
    signal clearRequested()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

            CachyHeading {
                level: 3
                text: qsTr("Lista de Reproducción")
                Layout.fillWidth: true
            }

            CachyToolbarButton {
                id: addBtn
                iconName: "list-add"
                tooltipText: qsTr("Añadir a la lista...")
                pillStyle: false
                onClicked: addMenu.open()

                CachyMenu {
                    id: addMenu
                    y: addBtn.height + 4

                    CachyMenuItem {
                        text: qsTr("Añadir archivo(s)...")
                        iconName: "document-open"
                        onTriggered: root.addFileRequested()
                    }

                    CachyMenuItem {
                        text: qsTr("Añadir carpeta / disco...")
                        iconName: "folder-open"
                        onTriggered: root.addFolderRequested()
                    }
                }
            }

            CachyToolbarButton {
                iconName: "edit-clear"
                tooltipText: qsTr("Vaciar lista")
                pillStyle: false
                onClicked: root.clearRequested()
            }
        }

        CachyDivider {
            Layout.fillWidth: true
        }

        ListView {
            id: playlistView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.playlist

            delegate: CachyListItem {
                id: itemDelegate
                width: ListView.view.width
                title: {
                    if (modelData.title && modelData.title.length > 0) return modelData.title;
                    if (modelData.filename) {
                        var parts = modelData.filename.split("/");
                        return parts[parts.length - 1];
                    }
                    return qsTr("Pista #%1").arg(index + 1);
                }
                subtitle: modelData.current ? qsTr("En reproducción") : ""
                statusText: modelData.current ? "▶" : ""
                selected: modelData.current || index === root.currentIndex

                onClicked: root.itemSelected(index)

                trailingActions: [
                    CachyToolbarButton {
                        iconName: "edit-delete"
                        tooltipText: qsTr("Eliminar")
                        pillStyle: false
                        onClicked: root.itemRemoved(index)
                    }
                ]
            }

            CachyPlaceholderMessage {
                anchors.centerIn: parent
                visible: root.playlist.length === 0
                text: qsTr("Lista vacía")
                iconName: "media-playlist-normal"
            }
        }
    }
}
