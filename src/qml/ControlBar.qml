import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CachyOS.UI
import org.cachyos.player

Item {
    id: root
    implicitHeight: 96

    property bool isPaused: false
    property double position: 0.0
    property double duration: 0.0
    property double volume: 100.0
    property bool isMuted: false
    property var chapters: []

    signal playPauseClicked()
    signal seekRequested(double position)
    signal seekRelative(double seconds)
    signal userVolumeChanged(double volume)
    signal muteToggled()
    signal openTracksMenu(string type, var targetItem)
    signal openPlaylist()
    signal openPreferences()
    signal openFilePicker()
    signal openFolderPicker()
    signal openRecentRequested(string path)
    signal toggleFullscreen()
    signal togglePip()

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
        width: Math.min(parent.width - 48, 720)
        height: 80
        radius: 20
        color: Qt.rgba(0.08, 0.09, 0.12, 0.88)
        border.color: Qt.rgba(1, 1, 1, 0.16)
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: 18
            anchors.rightMargin: 18
            anchors.topMargin: 8
            anchors.bottomMargin: 8
            spacing: 4

            // Top Row: Seekbar and Time Labels (uosc / QuickTime timeline)
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 22
                spacing: 12

                CachyLabel {
                    text: root.formatTime(root.position)
                    font.pixelSize: 11
                    font.bold: true
                    color: Qt.rgba(1, 1, 1, 0.85)
                    Layout.alignment: Qt.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 28
                    Layout.alignment: Qt.AlignVCenter

                    CachySlider {
                        id: seekSlider
                        anchors.fill: parent
                        from: 0.0
                        to: Math.max(root.duration, 1.0)
                        value: root.position

                        Binding {
                            target: seekSlider
                            property: "value"
                            value: root.position
                            when: !seekSlider.pressed
                        }

                        onMoved: {
                            root.seekRequested(value)
                        }
                    }

                    // Chapter Markers overlay (subtle hairlines on timeline)
                    Repeater {
                        model: (root.duration > 0 && root.chapters) ? root.chapters : []
                        delegate: Rectangle {
                            required property var modelData
                            readonly property double chapterTime: modelData.time ? Number(modelData.time) : 0.0
                            visible: chapterTime > 0 && chapterTime < root.duration
                            x: (chapterTime / root.duration) * (parent.width - 24) + 12
                            y: (parent.height - 6) / 2
                            width: 1.5
                            height: 6
                            radius: 0.75
                            color: Qt.rgba(1, 1, 1, 0.75)
                            z: 2
                        }
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
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 36

                // LEFT CLUSTER: Sources & Menus (Open, Subtitles, Audio, Playlist)
                Row {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 6

                    MediaButton {
                        id: openBtn
                        iconName: "document-open"
                        tooltipText: qsTr("Abrir...")
                        onClicked: openMenu.open()
                        anchors.verticalCenter: parent.verticalCenter

                        CachyMenu {
                            id: openMenu
                            y: -implicitHeight - 6

                            CachyMenuItem {
                                text: qsTr("Abrir archivo(s)...")
                                iconName: "document-open"
                                shortcutText: "Ctrl+O"
                                onTriggered: root.openFilePicker()
                            }

                            CachyMenuItem {
                                text: qsTr("Abrir carpeta...")
                                iconName: "folder-open"
                                shortcutText: "Ctrl+Shift+O"
                                onTriggered: root.openFolderPicker()
                            }

                            CachyMenuSeparator {
                                visible: PlayerSettings.recentFiles.length > 0
                            }

                            CachyMenu {
                                id: recentSubMenu
                                title: qsTr("Archivos recientes")
                                icon.name: "document-open-recent"
                                visible: PlayerSettings.recentFiles.length > 0

                                Instantiator {
                                    model: PlayerSettings.recentFiles
                                    delegate: CachyMenuItem {
                                        required property string modelData
                                        text: {
                                            var parts = modelData.split("/")
                                            return parts[parts.length - 1]
                                        }
                                        onTriggered: root.openRecentRequested(modelData)
                                    }
                                    onObjectAdded: (index, object) => recentSubMenu.addItem(object)
                                    onObjectRemoved: (index, object) => recentSubMenu.removeItem(object)
                                }
                            }
                        }
                    }

                    MediaButton {
                        id: subBtn
                        iconName: "media-view-subtitles"
                        tooltipText: qsTr("Subtítulos")
                        onClicked: root.openTracksMenu("sub", subBtn)
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        id: audioBtn
                        iconName: "audio-speakers"
                        tooltipText: qsTr("Pistas de audio")
                        onClicked: root.openTracksMenu("audio", audioBtn)
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        id: playlistBtn
                        iconName: "media-playlist-normal"
                        tooltipText: qsTr("Lista de reproducción")
                        onClicked: root.openPlaylist()
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                // CENTER CLUSTER: Perfectly Centered QuickTime Playback Transport (Seek -10s, Play/Pause, Seek +10s)
                Row {
                    anchors.centerIn: parent
                    spacing: 10

                    MediaButton {
                        iconName: "media-seek-backward"
                        tooltipText: qsTr("Retroceder 10s")
                        iconSize: 16
                        onClicked: root.seekRelative(-10)
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        primary: true
                        implicitWidth: 38
                        implicitHeight: 38
                        radius: 19
                        iconName: root.isPaused ? "media-playback-start" : "media-playback-pause"
                        iconSize: 20
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

                // RIGHT CLUSTER: Audio Volume with Tooltip, PiP, Fullscreen & Preferences
                Row {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 6

                    MediaButton {
                        iconName: root.isMuted || root.volume === 0 ? "audio-volume-muted" : "audio-volume-high"
                        tooltipText: root.isMuted ? qsTr("Reactivar sonido") : qsTr("Silenciar")
                        onClicked: root.muteToggled()
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    CachySlider {
                        id: volumeSlider
                        width: 90
                        anchors.verticalCenter: parent.verticalCenter
                        from: 0
                        to: 150
                        value: root.volume
                        onMoved: root.userVolumeChanged(value)

                        ToolTip.visible: hovered || pressed
                        ToolTip.text: qsTr("Volumen: %1%").arg(Math.round(value))
                        ToolTip.delay: 200
                    }

                    MediaButton {
                        iconName: "picture-in-picture"
                        tooltipText: qsTr("Picture in Picture (PiP)")
                        onClicked: root.togglePip()
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        iconName: "view-fullscreen"
                        tooltipText: qsTr("Pantalla completa")
                        onClicked: root.toggleFullscreen()
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MediaButton {
                        iconName: "configure"
                        tooltipText: qsTr("Ajustes")
                        onClicked: root.openPreferences()
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}
