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
    showTrafficLights: !isFullscreen
    showContentTitleBar: !isFullscreen
    showWindowFrame: !isFullscreen
    cornerRadius: isFullscreen ? 0 : 20

    headerCenter: CachyLabel {
        text: window.title
        font.pixelSize: 13
        font.weight: Font.DemiBold
        color: CachyPalette.text
        elide: Text.ElideMiddle
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.centerIn: parent
        width: Math.min(parent.width - 20, implicitWidth)
    }

    property bool isFullscreen: false

    // Standard mpv / macOS Keyboard Shortcuts
    Shortcut {
        sequence: "Space"
        onActivated: mpvController.togglePause()
    }
    Shortcut {
        sequence: "Left"
        onActivated: mpvController.seekRelative(-5)
    }
    Shortcut {
        sequence: "Right"
        onActivated: mpvController.seekRelative(5)
    }
    Shortcut {
        sequence: "Shift+Left"
        onActivated: mpvController.seekRelative(-30)
    }
    Shortcut {
        sequence: "Shift+Right"
        onActivated: mpvController.seekRelative(30)
    }
    Shortcut {
        sequence: "Up"
        onActivated: mpvController.volume = Math.min(150, mpvController.volume + 5)
    }
    Shortcut {
        sequence: "Down"
        onActivated: mpvController.volume = Math.max(0, mpvController.volume - 5)
    }
    Shortcut {
        sequence: "m"
        onActivated: mpvController.muted = !mpvController.muted
    }
    Shortcut {
        sequence: "f"
        onActivated: window.toggleFullscreen()
    }
    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (window.isFullscreen) window.toggleFullscreen()
        }
    }
    Shortcut {
        sequence: "["
        onActivated: mpvController.playbackSpeed = Math.max(0.25, mpvController.playbackSpeed - 0.1)
    }
    Shortcut {
        sequence: "]"
        onActivated: mpvController.playbackSpeed = Math.min(4.0, mpvController.playbackSpeed + 0.1)
    }
    Shortcut {
        sequence: "Backspace"
        onActivated: mpvController.playbackSpeed = 1.0
    }
    Shortcut {
        sequence: "s"
        onActivated: mpvController.takeScreenshot()
    }
    Shortcut {
        sequence: "Ctrl+O"
        onActivated: dialogHelper.openFiles()
    }
    Shortcut {
        sequence: "Ctrl+Shift+O"
        onActivated: dialogHelper.openFolder()
    }
    Shortcut {
        sequence: "Ctrl+,"
        onActivated: preferencesDialog.open()
    }


    MpvController {
        id: mpvController

        Component.onCompleted: {
            if (typeof initialFileUrl !== "undefined" && initialFileUrl.length > 0) {
                mpvController.openFile(initialFileUrl)
            }
        }
    }

    MprisManager {
        id: mprisManager
        controller: mpvController
        onRaiseRequested: {
            window.showNormal()
            window.requestActivate()
        }
        onQuitRequested: {
            Qt.quit()
        }
    }

    NativeDialogHelper {
        id: dialogHelper
        property bool isPlaylistMode: false

        onFolderSelected: (folderPath) => {
            if (isPlaylistMode) {
                mpvController.addFileToPlaylist(folderPath)
            } else {
                mpvController.openFiles([folderPath])
            }
            isPlaylistMode = false
        }

        onFilesSelected: (filePaths) => {
            if (isPlaylistMode) {
                for (var i = 0; i < filePaths.length; ++i) {
                    mpvController.addFileToPlaylist(filePaths[i])
                }
            } else {
                mpvController.openFiles(filePaths)
            }
            isPlaylistMode = false
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

        // Dedicated black canvas behind video when playing video
        Rectangle {
            anchors.fill: parent
            color: "#000000"
            visible: mpvController.hasVideo
            z: -1
        }

        DropArea {
            anchors.fill: parent
            onEntered: (drag) => {
                if (drag.hasUrls) drag.acceptProposedAction()
            }
            onDropped: (drop) => {
                if (drop.hasUrls && drop.urls.length > 0) {
                    drop.acceptProposedAction()
                    mpvController.openFiles(drop.urls)
                }
            }
        }

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
            visible: mpvController.hasVideo
            z: 0
        }

        // Audio / Music Presentation Surface
        ColumnLayout {
            anchors.centerIn: parent
            visible: !mpvController.hasVideo && mpvController.duration > 0
            spacing: 16

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: Math.min(window.width * 0.45, 280)
                height: width
                radius: 16
                color: CachyPalette.cardBackground
                border.color: CachyPalette.hardDividerColor
                border.width: 1
                clip: true

                Image {
                    anchors.fill: parent
                    anchors.margins: 1
                    source: mpvController.coverArtPath
                    fillMode: Image.PreserveAspectCrop
                    visible: mpvController.coverArtPath.length > 0
                    mipmap: true
                }

                CachyIcon {
                    anchors.centerIn: parent
                    name: "audio-x-generic"
                    size: 64
                    color: CachyPalette.textSecondary
                    visible: mpvController.coverArtPath.length === 0
                }
            }

            CachyHeading {
                Layout.alignment: Qt.AlignHCenter
                level: 2
                text: mpvController.mediaTitle
                color: CachyPalette.text
                elide: Text.ElideMiddle
                Layout.maximumWidth: Math.min(window.width - 80, 500)
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
                color: CachyPalette.textSecondary
            }

            CachyHeading {
                Layout.alignment: Qt.AlignHCenter
                level: 2
                text: qsTr("Arrastra o abre un archivo multimedia")
                color: CachyPalette.text
            }

            CachyButton {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Seleccionar archivo...")
                iconName: "document-open"
                onClicked: {
                    dialogHelper.isPlaylistMode = false
                    dialogHelper.openFiles()
                }
            }
        }

        // Bottom Floating Controls (QuickTime & uosc ergonomics)
        ControlBar {
            id: controlBar
            z: 10
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
            onOpenFilePicker: {
                dialogHelper.isPlaylistMode = false
                dialogHelper.openFiles()
            }
            onOpenFolderPicker: {
                dialogHelper.isPlaylistMode = false
                dialogHelper.openFolder()
            }
            onToggleFullscreen: window.toggleFullscreen()
            onOpenPreferences: preferencesDialog.open()

            onOpenPlaylist: playlistDrawer.open()
            onOpenTracksMenu: (type, targetItem) => {
                trackMenu.trackType = type
                trackMenu.trackList = mpvController.trackList
                trackMenu.currentVideoPath = mpvController.currentPath
                if (targetItem) {
                    trackMenu.preferredEdge = Qt.TopEdge
                    trackMenu.openAt(targetItem)
                } else {
                    trackMenu.open()
                }
            }
        }
    }

    // Native multi-file picker
    FileDialog {
        id: fileDialog
        title: qsTr("Abrir vídeo(s) o audio(s)")
        fileMode: FileDialog.OpenFiles
        nameFilters: [
            qsTr("Archivos multimedia (*.mp4 *.mkv *.avi *.mov *.webm *.flac *.mp3 *.wav *.ogg *.opus *.m4v *.ts *.m2ts)"),
            qsTr("Todos los archivos (*)")
        ]
        onAccepted: {
            if (fileDialog.selectedFiles && fileDialog.selectedFiles.length > 0) {
                mpvController.openFiles(fileDialog.selectedFiles)
            } else if (fileDialog.selectedFile) {
                mpvController.openFile(fileDialog.selectedFile)
            }
        }
    }

    // Native folder picker (e.g. whole season or music album)
    FolderDialog {
        id: folderDialog
        title: qsTr("Abrir carpeta / temporada")
        onAccepted: {
            if (folderDialog.selectedFolder) {
                mpvController.openFiles([folderDialog.selectedFolder])
            }
        }
    }

    
        // macOS / uosc HUD Pill (Volume, Speed, Seek indicator)
        Rectangle {
            id: hudPill
            anchors.top: parent.top
            anchors.topMargin: window.isFullscreen ? 24 : 54
            anchors.horizontalCenter: parent.horizontalCenter
            width: hudLayout.implicitWidth + 28
            height: 36
            radius: 18
            color: Qt.rgba(0.08, 0.09, 0.12, 0.88)
            border.color: Qt.rgba(1, 1, 1, 0.18)
            border.width: 1
            z: 20
            opacity: 0.0

            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }

            Timer {
                id: hudTimer
                interval: 1200
                repeat: false
                onTriggered: hudPill.opacity = 0.0
            }

            RowLayout {
                id: hudLayout
                anchors.centerIn: parent
                spacing: 8

                CachyIcon {
                    id: hudIcon
                    name: "audio-volume-high"
                    size: 16
                    color: "white"
                }

                CachyLabel {
                    id: hudLabel
                    text: ""
                    font.pixelSize: 12
                    font.bold: true
                    color: "white"
                }
            }

            function show(icon, text) {
                hudIcon.name = icon
                hudLabel.text = text
                hudPill.opacity = 1.0
                hudTimer.restart()
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

    PlaylistDrawer {
        id: playlistDrawer
        playlist: mpvController.playlist
        currentIndex: mpvController.playlistCurrentIndex
        onItemSelected: (index) => mpvController.playPlaylistItem(index)
        onItemRemoved: (index) => mpvController.removePlaylistItem(index)
        onClearRequested: mpvController.clearPlaylist()
        onAddFileRequested: {
            dialogHelper.isPlaylistMode = true
            dialogHelper.openFiles()
        }
        onAddFolderRequested: {
            dialogHelper.isPlaylistMode = true
            dialogHelper.openFolder()
        }
    }

    FileDialog {
        id: playlistFileDialog
        title: qsTr("Añadir archivo(s) a la lista")
        fileMode: FileDialog.OpenFiles
        nameFilters: [qsTr("Archivos multimedia (*.mp4 *.mkv *.avi *.mov *.webm *.flac *.mp3 *.ogg)"), qsTr("Todos los archivos (*)")]
        onAccepted: {
            if (playlistFileDialog.selectedFiles && playlistFileDialog.selectedFiles.length > 0) {
                for (var i = 0; i < playlistFileDialog.selectedFiles.length; ++i) {
                    mpvController.addFileToPlaylist(playlistFileDialog.selectedFiles[i].toString())
                }
            } else if (playlistFileDialog.selectedFile) {
                mpvController.addFileToPlaylist(playlistFileDialog.selectedFile.toString())
            }
        }
    }


    Connections {
        target: mpvController
        function onVolumeChanged() {
            if (mpvController.duration > 0) {
                var icon = mpvController.muted || mpvController.volume === 0 ? "audio-volume-muted" : "audio-volume-high"
                hudPill.show(icon, qsTr("Volumen: %1%").arg(Math.round(mpvController.volume)))
            }
        }
        function onPlaybackSpeedChanged() {
            if (mpvController.duration > 0) {
                hudPill.show("speedometer", qsTr("Velocidad: %1x").arg(mpvController.playbackSpeed.toFixed(2)))
            }
        }
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
