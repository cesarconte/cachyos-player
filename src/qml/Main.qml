import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CachyOS.UI
import org.cachyos.player

CachyWindow {
    id: window
    width: 1080
    height: 640
    minimumWidth: 520
    minimumHeight: 340
    title: mpvController.mediaTitle.length > 0 ? mpvController.mediaTitle + " - CachyOS Player" : "CachyOS Player"
    showTitleBarDivider: false

    property bool isFullscreen: false

    MpvController {
        id: mpvController

        Component.onCompleted: {
            if (typeof initialFileUrl !== "undefined" && initialFileUrl.length > 0) {
                mpvController.openFile(initialFileUrl)
            }
        }
    }

    // Auto-hide UI timer on inactivity
    Timer {
        id: hideUiTimer
        interval: 3000
        running: !mpvController.paused && mpvController.duration > 0
        repeat: false
        onTriggered: {
            if (!controlBar.containsMouse) {
                controlBar.opacity = 0.0
            }
        }
    }

    MouseArea {
        id: globalMouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPositionChanged: {
            controlBar.opacity = 1.0
            hideUiTimer.restart()
        }

        onDoubleClicked: {
            window.toggleFullscreen()
        }

        onClicked: (mouse) => {
            if (mouse.button === Qt.LeftButton) {
                mpvController.togglePause()
            }
        }

        onWheel: (wheel) => {
            if (wheel.angleDelta.y > 0) {
                mpvController.volume = Math.min(150, mpvController.volume + 5)
            } else if (wheel.angleDelta.y < 0) {
                mpvController.volume = Math.max(0, mpvController.volume - 5)
            }
        }

        // Mpv video surface
        MpvItem {
            id: mpvItem
            anchors.fill: parent
            controller: mpvController
        }

        // Music Cover Art View
        Image {
            id: coverArtImage
            anchors.centerIn: parent
            width: Math.min(parent.width * 0.45, 340)
            height: width
            fillMode: Image.PreserveAspectFit
            source: mpvController.coverArtPath
            visible: mpvController.coverArtPath.length > 0 && !mpvController.hasVideo
            mipmap: true

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: Qt.rgba(1, 1, 1, 0.18)
                border.width: 1
                radius: 12
            }
        }

        // Placeholder when idle
        ColumnLayout {
            anchors.centerIn: parent
            visible: mpvController.duration === 0 && mpvController.coverArtPath.length === 0
            spacing: 12

            CachyIcon {
                Layout.alignment: Qt.AlignHCenter
                name: "multimedia-video-player"
                size: 64
                color: Qt.rgba(1, 1, 1, 0.4)
            }

            CachyHeading {
                Layout.alignment: Qt.AlignHCenter
                level: 2
                text: qsTr("Arrastra o abre un archivo multimedia")
                color: Qt.rgba(1, 1, 1, 0.6)
            }

            CachyButton {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Seleccionar archivo...")
                iconName: "document-open"
                onClicked: fileDialog.open()
            }
        }

        // Bottom Floating Controls (QuickTime & uosc ergonomics)
        ControlBar {
            id: controlBar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottomMargin: 16
            opacity: 1.0

            Behavior on opacity {
                NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
            }

            isPaused: mpvController.paused
            position: mpvController.position
            duration: mpvController.duration
            volume: mpvController.volume
            isMuted: mpvController.muted

            onPlayPauseClicked: mpvController.togglePause()
            onSeekRequested: (pos) => mpvController.position = pos
            onSeekRelative: (sec) => mpvController.seekRelative(sec)
            onUserVolumeChanged: (vol) => mpvController.volume = vol
            onMuteToggled: mpvController.muted = !mpvController.muted
            onOpenFilePicker: fileDialog.open()
            onToggleFullscreen: window.toggleFullscreen()
            onOpenPreferences: preferencesDialog.open()

            onOpenTracksMenu: (type, targetItem) => {
                trackMenu.trackType = type
                trackMenu.trackList = mpvController.trackList
                trackMenu.currentVideoPath = mpvController.currentPath
                trackMenu.open()
            }
        }
    }

    // Native file picker
    FileDialog {
        id: fileDialog
        title: qsTr("Abrir vídeo o audio")
        nameFilters: [
            qsTr("Archivos multimedia (*.mp4 *.mkv *.avi *.mov *.webm *.flac *.mp3 *.wav *.ogg)"),
            qsTr("Todos los archivos (*)")
        ]
        onAccepted: {
            mpvController.openFile(fileDialog.selectedFile)
        }
    }

    TrackMenu {
        id: trackMenu
        onTrackSelected: (type, id) => {
            mpvController.selectTrack(type, id)
        }
    }

    PreferencesDialog {
        id: preferencesDialog
    }

    function toggleFullscreen() {
        if (isFullscreen) {
            window.showNormal()
            isFullscreen = false
        } else {
            window.showFullScreen()
            isFullscreen = true
        }
    }
}
