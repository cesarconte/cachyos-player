import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI

Item {
    id: root
    height: 84

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
        width: Math.min(parent.width - 48, 760)
        height: 68
        radius: 34
        color: Qt.rgba(0.08, 0.09, 0.11, 0.82)
        border.color: Qt.rgba(1, 1, 1, 0.14)
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.topMargin: 8
            anchors.bottomMargin: 8
            spacing: 2

            // Seekbar and Time Labels
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                CachyLabel {
                    text: root.formatTime(root.position)
                    font.pixelSize: 11
                    color: Qt.rgba(1, 1, 1, 0.75)
                }

                CachySlider {
                    id: seekSlider
                    Layout.fillWidth: true
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
                    color: Qt.rgba(1, 1, 1, 0.75)
                }
            }

            // Media & Tool Buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 14

                // Open File
                CachyToolbarButton {
                    iconName: "document-open"
                    tooltipText: qsTr("Abrir archivo...")
                    onClicked: root.openFilePicker()
                }

                // Subtitles menu
                CachyToolbarButton {
                    id: subBtn
                    iconName: "media-view-subtitles"
                    tooltipText: qsTr("Subtítulos")
                    onClicked: root.openTracksMenu("sub", subBtn)
                }

                // Audio menu
                CachyToolbarButton {
                    id: audioBtn
                    iconName: "audio-speakers"
                    tooltipText: qsTr("Pistas de audio")
                    onClicked: root.openTracksMenu("audio", audioBtn)
                }

                Item { Layout.fillWidth: true }

                // Play / Pause central button
                Rectangle {
                    width: 36
                    height: 36
                    radius: 18
                    color: Qt.rgba(1, 1, 1, 0.12)
                    border.color: Qt.rgba(1, 1, 1, 0.22)
                    border.width: 1

                    CachyIcon {
                        anchors.centerIn: parent
                        name: root.isPaused ? "media-playback-start" : "media-playback-pause"
                        size: 18
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.playPauseClicked()
                    }
                }

                Item { Layout.fillWidth: true }

                // Volume button & slider
                CachyToolbarButton {
                    iconName: root.isMuted || root.volume === 0 ? "audio-volume-muted" : "audio-volume-high"
                    tooltipText: qsTr("Silenciar")
                    onClicked: root.muteToggled()
                }

                CachySlider {
                    id: volumeSlider
                    Layout.preferredWidth: 80
                    from: 0
                    to: 150
                    value: root.volume
                    onMoved: root.userVolumeChanged(value)
                }

                // Fullscreen
                CachyToolbarButton {
                    iconName: "view-fullscreen"
                    tooltipText: qsTr("Pantalla completa")
                    onClicked: root.toggleFullscreen()
                }

                // Preferences
                CachyToolbarButton {
                    iconName: "configure"
                    tooltipText: qsTr("Ajustes")
                    onClicked: root.openPreferences()
                }
            }
        }
    }
}
