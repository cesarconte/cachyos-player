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
    signal seekRelative(double seconds)
    signal userVolumeChanged(double volume)
    signal muteToggled()
    signal openTracksMenu(string type, var targetItem)
    signal openPlaylist()
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

    // Floating Capsule Background (macOS Liquid Glass / QuickTime / uosc style)
    Rectangle {
        id: capsule
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 640)
        height: 76
        radius: 20
        color: Qt.rgba(0.08, 0.09, 0.12, 0.88)
        border.color: Qt.rgba(1, 1, 1, 0.16)
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            anchors.topMargin: 8
            anchors.bottomMargin: 8
            spacing: 4

            // Top Row: Seekbar and Time Labels (uosc / QuickTime timeline)
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 22
                spacing: 10

                CachyLabel {
                    text: root.formatTime(root.position)
                    font.pixelSize: 11
                    font.bold: true
                    color: Qt.rgba(1, 1, 1, 0.85)
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
                    font.bold: true
                    color: Qt.rgba(1, 1, 1, 0.85)
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            // Bottom Row: Ergonomic Transport & Tools Bar (QuickTime distribution)
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                spacing: 6

                // LEFT CLUSTER: Sources & Menus (Open, Subtitles, Audio)
                MediaButton {
                    iconName: "document-open"
                    tooltipText: qsTr("Abrir archivo...")
                    onClicked: root.openFilePicker()
                    Layout.alignment: Qt.AlignVCenter
                }

                MediaButton {
                    id: subBtn
                    iconName: "media-view-subtitles"
                    tooltipText: qsTr("Subtítulos")
                    onClicked: root.openTracksMenu("sub", subBtn)
                    Layout.alignment: Qt.AlignVCenter
                }

                MediaButton {
                    id: audioBtn
                    iconName: "audio-speakers"
                    tooltipText: qsTr("Pistas de audio")
                    onClicked: root.openTracksMenu("audio", audioBtn)
                    Layout.alignment: Qt.AlignVCenter
                }

                MediaButton {
                    id: playlistBtn
                    iconName: "media-playlist-normal"
                    tooltipText: qsTr("Lista de reproducción")
                    onClicked: root.openPlaylist()
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                // CENTER CLUSTER: QuickTime Playback Transport (Seek -10s, Play/Pause, Seek +10s)
                Row {
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 8

                    MediaButton {
                        iconName: "media-seek-backward"
                        tooltipText: qsTr("Retroceder 10s")
                        iconSize: 16
                        onClicked: root.seekRelative(-10)
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        primary: true
                        implicitWidth: 36
                        implicitHeight: 36
                        radius: 18
                        iconName: root.isPaused ? "media-playback-start" : "media-playback-pause"
                        iconSize: 18
                        tooltipText: root.isPaused ? qsTr("Reproducir") : qsTr("Pausar")
                        onClicked: root.playPauseClicked()
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        iconName: "media-seek-forward"
                        tooltipText: qsTr("Avanzar 10s")
                        iconSize: 16
                        onClicked: root.seekRelative(10)
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Item { Layout.fillWidth: true }

                // RIGHT CLUSTER: Audio Volume, Fullscreen & Preferences
                MediaButton {
                    iconName: root.isMuted || root.volume === 0 ? "audio-volume-muted" : "audio-volume-high"
                    tooltipText: qsTr("Silenciar")
                    onClicked: root.muteToggled()
                    Layout.alignment: Qt.AlignVCenter
                }

                CachySlider {
                    id: volumeSlider
                    Layout.preferredWidth: 64
                    Layout.alignment: Qt.AlignVCenter
                    from: 0
                    to: 150
                    value: root.volume
                    onMoved: root.userVolumeChanged(value)
                }

                MediaButton {
                    iconName: "view-fullscreen"
                    tooltipText: qsTr("Pantalla completa")
                    onClicked: root.toggleFullscreen()
                    Layout.alignment: Qt.AlignVCenter
                }

                MediaButton {
                    iconName: "configure"
                    tooltipText: qsTr("Ajustes")
                    onClicked: root.openPreferences()
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }
    }
}
