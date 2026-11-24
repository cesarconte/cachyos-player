import QtQuick
import QtQuick.Controls
import CachyOS.UI

Rectangle {
    id: root

    property string iconName: ""
    property string tooltipText: ""
    property int iconSize: 18
    property color iconColor: "#FFFFFF"
    property bool checkable: false
    property bool checked: false
    property bool primary: false

    signal clicked()

    implicitWidth: 32
    implicitHeight: 32
    radius: 16

    color: {
        if (root.primary) return Qt.rgba(1, 1, 1, 0.22);
        if (root.checked) return Qt.rgba(1, 1, 1, 0.25);
        if (btnTap.pressed) return Qt.rgba(1, 1, 1, 0.20);
        if (btnHover.hovered) return Qt.rgba(1, 1, 1, 0.12);
        return "transparent";
    }

    border.width: root.primary || root.checked ? 1 : 0
    border.color: Qt.rgba(1, 1, 1, 0.28)

    Behavior on color { ColorAnimation { duration: 120 } }
    Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutBack } }
    scale: btnTap.pressed ? 0.94 : (btnHover.hovered ? 1.06 : 1.0)

    CachyIcon {
        anchors.centerIn: parent
        name: root.iconName
        size: root.iconSize
        color: root.iconColor
        colored: true
    }

    HoverHandler {
        id: btnHover
        cursorShape: Qt.PointingHandCursor
    }

    TapHandler {
        id: btnTap
        onTapped: {
            if (root.checkable) {
                root.checked = !root.checked;
            }
            root.clicked();
        }
    }

    ToolTip.visible: btnHover.hovered && root.tooltipText.length > 0
    ToolTip.text: root.tooltipText
    ToolTip.delay: 400
}
