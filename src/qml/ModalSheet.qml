/*
 * SPDX-FileCopyrightText: 2026 CachyOS UI contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import CachyOS.UI

Item {
    id: root

    property string title: ""
    property string subtitle: ""
    property int preferredWidth: 720
    property bool closeOnScrimClick: true
    default property alias content: bodyColumn.data
    property alias footerContent: footerHost.data

    signal closed()
    signal opened()

    function open() {
        root.visible = true;
        root.forceActiveFocus();
        root.opened();
    }

    function close() {
        root.visible = false;
        root.closed();
    }

    anchors.fill: parent
    visible: false
    z: 1000

    Keys.onEscapePressed: root.close()

    // Squircle-matched modal scrim
    Rectangle {
        anchors.fill: parent
        radius: CachyTheme.radiusWindow
        color: Qt.rgba(0, 0, 0, 0.45)

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onClicked: (mouse) => {
                if (!root.closeOnScrimClick) {
                    return;
                }
                if (mouse.x < card.x || mouse.x > (card.x + card.width) ||
                    mouse.y < card.y || mouse.y > (card.y + card.height)) {
                    root.close();
                }
            }
            onWheel: (wheel) => wheel.accepted = true
        }
    }

    // Centered Liquid Glass Sheet Card
    Rectangle {
        id: card
        anchors.centerIn: parent
        width: Math.min(Math.max(320, parent.width - 48), root.preferredWidth)
        height: Math.min(parent.height - 48, cardLayout.implicitHeight + (CachyTheme.marginWindow * 2))
        radius: CachyTheme.radiusLarge
        color: CachyPalette.sheetBackground
        border.width: 1
        border.color: CachyPalette.specularBorder

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            hoverEnabled: true
            preventStealing: true
            onWheel: (wheel) => wheel.accepted = true
        }

        ColumnLayout {
            id: cardLayout
            anchors.fill: parent
            anchors.margins: CachyTheme.marginWindow
            spacing: CachyTheme.spacingMedium

            RowLayout {
                Layout.fillWidth: true
                spacing: CachyTheme.spacingSmall

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    spacing: 2

                    CachyHeading {
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        text: root.title
                        level: 3
                    }

                    Text {
                        visible: root.subtitle.length > 0
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        text: root.subtitle
                        font.pixelSize: 12
                        color: CachyPalette.textSecondary
                        wrapMode: Text.WordWrap
                    }
                }

                CachyToolbarButton {
                    iconName: "window-close"
                    tooltipText: qsTr("Cerrar")
                    onClicked: root.close()
                }
            }

            CachyDivider { Layout.fillWidth: true }

            ColumnLayout {
                id: bodyColumn
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: CachyTheme.spacingSmall
            }

            RowLayout {
                id: footerHost
                Layout.fillWidth: true
                spacing: CachyTheme.spacingSmall
                visible: footerHost.children.length > 0
            }
        }
    }
}
