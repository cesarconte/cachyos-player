import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI

Item {
    id: root
    implicitHeight: 96

    property bool isPaused: false
    property double position: 0.0
    property double duration: 0.0
    property double volume: 100.0
    property bool isMuted: false

    signal playPauseClicked()
    signal seekRequested(double position)
    signal userVolumeChanged(double volume)
    signal muteToggled()
    signal openTracksMenu(string type, var targetItem)
    signal openPreferences()
    signal openFilePicker()
    signal toggleFullscreen()

    function formatTime(seconds) {
        if (isNaN(seconds) || seconds < 0) return "00:00"
        var s = Math.floor(seconds)
        var m = Math.floor(s / 60)
        var h = Math.floor(m / 60)
        s = s % 60
        m = m % 60
        if (h > 0) {
            return (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s
        }
        return (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s
    }

    // Floating Capsule Background (macOS Liquid Glass / QuickTime style)
    Rectangle {
        id: capsule
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 680)
        height: 82
        radius: 20
        color: Qt.rgba(0.08, 0.09, 0.12, 0.88)
        border.color: Qt.rgba(1, 1, 1, 0.16)
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 6

            // Seekbar and Time Labels
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 24
                spacing: 10

                CachyLabel {
                    text: root.formatTime(root.position)
                    font.pixelSize: 11
                    color: Qt.rgba(1, 1, 1, 0.8)
                    Layout.alignment: Qt.AlignVCenter
                }

                CachySlider {
                    id: seekSlider
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    from: 0.0
                    to: Math.max(root.duration, 1.0)
                    value: root.position

                    onMoved: {
                        root.seekRequested(value)
                    }
                }

                CachyLabel {
                    text: root.formatTime(root.duration)
                    font.pixelSize: 11
                    color: Qt.rgba(1, 1, 1, 0.8)
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            // Media & Tool Buttons
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                // Open File
                CachyToolbarButton {
                    iconName: "document-open"
                    tooltipText: qsTr("Abrir archivo...")
                    pillStyle: false
                    onClicked: root.openFilePicker()
                    Layout.alignment: Qt.AlignVCenter
                }

                // Subtitles menu
                CachyToolbarButton {
                    id: subBtn
                    iconName: "media-view-subtitles"
                    tooltipText: qsTr("Subtítulos")
                    pillStyle: false
                    onClicked: root.openTracksMenu("sub", subBtn)
                    Layout.alignment: Qt.AlignVCenter
                }

                // Audio menu
                CachyToolbarButton {
                    id: audioBtn
                    iconName: "audio-speakers"
                    tooltipText: qsTr("Pistas de audio")
                    pillStyle: false
                    onClicked: root.openTracksMenu("audio", audioBtn)
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                // Play / Pause central button (macOS QuickTime Glass Orb)
                Rectangle {
                    Layout.alignment: Qt.AlignVCenter
                    width: 32
                    height: 32
                    radius: 16
                    color: playHover.hovered ? Qt.rgba(1, 1, 1, 0.22) : Qt.rgba(1, 1, 1, 0.14)
                    border.color: Qt.rgba(1, 1, 1, 0.28)
                    border.width: 1

                    CachyIcon {
                        anchors.centerIn: parent
                        name: root.isPaused ? "media-playback-start" : "media-playback-pause"
                        size: 16
                        color: "white"
                    }

                    HoverHandler {
                        id: playHover
                        cursorShape: Qt.PointingHandCursor
                    }

                    TapHandler {
                        onTapped: root.playPauseClicked()
                    }
                }

                Item { Layout.fillWidth: true }

                // Volume button & slider
                CachyToolbarButton {
                    iconName: root.isMuted || root.volume === 0 ? "audio-volume-muted" : "audio-volume-high"
                    tooltipText: qsTr("Silenciar")
                    pillStyle: false
                    onClicked: root.muteToggled()
                    Layout.alignment: Qt.AlignVCenter
                }

                CachySlider {
                    id: volumeSlider
                    Layout.preferredWidth: 70
                    Layout.alignment: Qt.AlignVCenter
                    from: 0
                    to: 150
                    value: root.volume
                    onMoved: root.userVolumeChanged(value)
                }

                // Fullscreen
                CachyToolbarButton {
                    iconName: "view-fullscreen"
                    tooltipText: qsTr("Pantalla completa")
                    pillStyle: false
                    onClicked: root.toggleFullscreen()
                    Layout.alignment: Qt.AlignVCenter
                }

                // Preferences
                CachyToolbarButton {
                    iconName: "configure"
                    tooltipText: qsTr("Ajustes")
                    pillStyle: false
                    onClicked: root.openPreferences()
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }
    }
}
