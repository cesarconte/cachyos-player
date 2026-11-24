import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as Platform
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
    showTrafficLights: !isFullscreen && !isPip
    showContentTitleBar: !isFullscreen && !isPip
    showWindowFrame: !isFullscreen && !isPip
    cornerRadius: isFullscreen ? 0 : (isPip ? 14 : 20)
    flags: isPip ? (Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint) : (Qt.Window | Qt.FramelessWindowHint)

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
    property bool isPip: false
    property var savedGeometry: ({ width: 1080, height: 640, x: 100, y: 100 })
    property double currentIntroStart: 0.0
    property double currentIntroEnd: 0.0

    Component.onCompleted: {
        var s = PlayerSettings
        var savedW = s.windowWidth
        var savedH = s.windowHeight
        if (savedW > 400 && savedH > 300) {
            window.width = savedW
            window.height = savedH
        }
    }

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()

    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        repeat: false
        onTriggered: {
            if (!isFullscreen && !isPip && window.width > 400 && window.height > 300) {
                PlayerSettings.windowWidth = window.width
                PlayerSettings.windowHeight = window.height
            }
        }
    }

    // Native macOS / KDE Plasma Global Menu Bar (QuickTime Player structure)
    Platform.MenuBar {
        Platform.Menu {
            title: qsTr("&Archivo")

            Platform.MenuItem {
                text: qsTr("Abrir archivo(s)...")
                shortcut: "Ctrl+O"
                onTriggered: {
                    dialogHelper.isPlaylistMode = false
                    dialogHelper.openFiles()
                }
            }

            Platform.MenuItem {
                text: qsTr("Abrir carpeta / temporada...")
                shortcut: "Ctrl+Shift+O"
                onTriggered: {
                    dialogHelper.isPlaylistMode = false
                    dialogHelper.openFolder()
                }
            }

            Platform.Menu {
                id: globalRecentMenu
                title: qsTr("Abrir recientes")
                enabled: PlayerSettings.recentFiles.length > 0

                Instantiator {
                    model: PlayerSettings.recentFiles
                    delegate: Platform.MenuItem {
                        required property string modelData
                        text: {
                            var parts = modelData.split("/")
                            return parts[parts.length - 1]
                        }
                        onTriggered: mpvController.openFile(modelData)
                    }
                    onObjectAdded: (index, object) => globalRecentMenu.addItem(object)
                    onObjectRemoved: (index, object) => globalRecentMenu.removeItem(object)
                }
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Cerrar")
                shortcut: "Ctrl+W"
                onTriggered: mpvController.stop()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Salir")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }

        Platform.Menu {
            title: qsTr("&Reproducción")

            Platform.MenuItem {
                text: mpvController.paused ? qsTr("Reproducir") : qsTr("Pausar")
                shortcut: "Space"
                onTriggered: mpvController.togglePause()
            }

            Platform.MenuItem {
                text: qsTr("Detener")
                onTriggered: mpvController.stop()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Avanzar 10 segundos")
                shortcut: "Right"
                onTriggered: mpvController.seekRelative(10)
            }

            Platform.MenuItem {
                text: qsTr("Retroceder 10 segundos")
                shortcut: "Left"
                onTriggered: mpvController.seekRelative(-10)
            }

            Platform.MenuItem {
                text: qsTr("Capítulo siguiente")
                shortcut: "Shift+N"
                onTriggered: mpvController.nextChapter()
            }

            Platform.MenuItem {
                text: qsTr("Capítulo anterior")
                shortcut: "Shift+P"
                onTriggered: mpvController.previousChapter()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Subir volumen")
                shortcut: "Up"
                onTriggered: mpvController.volume = Math.min(150, mpvController.volume + 5)
            }

            Platform.MenuItem {
                text: qsTr("Bajar volumen")
                shortcut: "Down"
                onTriggered: mpvController.volume = Math.max(0, mpvController.volume - 5)
            }

            Platform.MenuItem {
                text: mpvController.muted ? qsTr("Reactivar sonido") : qsTr("Silenciar")
                shortcut: "M"
                onTriggered: mpvController.muted = !mpvController.muted
            }
        }

        Platform.Menu {
            title: qsTr("&Visualización")

            Platform.MenuItem {
                text: window.isFullscreen ? qsTr("Salir de pantalla completa") : qsTr("Pantalla completa")
                shortcut: "F"
                onTriggered: window.toggleFullscreen()
            }

            Platform.MenuItem {
                text: window.isPip ? qsTr("Desactivar Picture in Picture") : qsTr("Picture in Picture (Siempre visible)")
                shortcut: "P"
                onTriggered: window.togglePip()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Lista de reproducción")
                onTriggered: playlistDrawer.open()
            }

            Platform.MenuItem {
                text: qsTr("Capturar fotograma")
                shortcut: "S"
                onTriggered: mpvController.takeScreenshot()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                text: qsTr("Preferencias...")
                shortcut: "Ctrl+,"
                onTriggered: preferencesDialog.open()
            }
        }
    }

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
    Shortcut {
        sequence: "p"
        onActivated: togglePip()
    }


    MpvController {
        id: mpvController

        Component.onCompleted: {
            if (typeof initialFileUrl !== "undefined" && initialFileUrl.length > 0) {
                mpvController.openFile(initialFileUrl)
            }
        }

        onChapterListChanged: {
            // Analizar capítulos para detectar si existe intro / cabecera / opening
            window.detectIntroRange()
        }

        onPositionChanged: {
            // Auto skip si está activado en ajustes
            if (PlayerSettings.autoSkipIntro && currentIntroEnd > 0) {
                if (mpvController.position >= currentIntroStart && mpvController.position < currentIntroEnd) {
                    mpvController.position = currentIntroEnd + 0.5
                    hudPill.show("media-seek-forward", qsTr("Intro saltada automáticamente"))
                }
            }
        }
    }

    function detectIntroRange() {
        currentIntroStart = 0.0
        currentIntroEnd = 0.0
        var chs = mpvController.chapterList
        if (!chs || chs.length === 0) return

        var introPattern = /(intro|opening|op\b|cabecera|prologue|title sequence)/i
        for (var i = 0; i < chs.length; ++i) {
            var title = (chs[i].title || "").trim()
            var start = chs[i].time || 0.0
            var end = (i + 1 < chs.length) ? (chs[i + 1].time || 0.0) : (start + 90.0)

            if (introPattern.test(title)) {
                currentIntroStart = start
                currentIntroEnd = end
                return
            }
        }
    }

    Connections {
        target: PlayerSettings
        function onScaleChanged() { mpvController.setProperty("scale", PlayerSettings.scale) }
        function onCscaleChanged() { mpvController.setProperty("cscale", PlayerSettings.cscale) }
        function onHwdecChanged() { mpvController.setProperty("hwdec", PlayerSettings.hwdec) }
        function onDebandChanged() { mpvController.setProperty("deband", PlayerSettings.deband) }
        function onInterpolationChanged() { mpvController.setProperty("interpolation", PlayerSettings.interpolation) }
        function onVideoSyncChanged() { mpvController.setProperty("video-sync", PlayerSettings.videoSync) }
        function onSubScaleChanged() { mpvController.setProperty("sub-scale", PlayerSettings.subScale) }
        function onSubFontChanged() { mpvController.setProperty("sub-font", PlayerSettings.subFont) }
        function onSubFontSizeChanged() { mpvController.setProperty("sub-font-size", PlayerSettings.subFontSize) }
        function onSubColorChanged() { mpvController.setProperty("sub-color", PlayerSettings.subColor) }
        function onSubBoldChanged() { mpvController.setProperty("sub-bold", PlayerSettings.subBold) }
        function onSubItalicChanged() { mpvController.setProperty("sub-italic", PlayerSettings.subItalic) }
        function onSubSpacingChanged() { mpvController.setProperty("sub-spacing", PlayerSettings.subSpacing) }
        function onSubLineSpacingChanged() { mpvController.setProperty("sub-line-spacing", PlayerSettings.subLineSpacing) }
        function onSubBorderSizeChanged() { mpvController.setProperty("sub-border-size", PlayerSettings.subBorderSize) }
        function onSubBorderColorChanged() { mpvController.setProperty("sub-border-color", PlayerSettings.subBorderColor) }
        function onSubShadowOffsetChanged() { mpvController.setProperty("sub-shadow-offset", PlayerSettings.subShadowOffset) }
        function onSubShadowColorChanged() { mpvController.setProperty("sub-shadow-color", PlayerSettings.subShadowColor) }
        function onSubBlurChanged() { mpvController.setProperty("sub-blur", PlayerSettings.subBlur) }
        function onSubMarginYChanged() { mpvController.setProperty("sub-margin-y", PlayerSettings.subMarginY) }
        function onBlendSubtitlesChanged() { mpvController.setProperty("blend-subtitles", PlayerSettings.blendSubtitles) }
        function onSlangChanged() { mpvController.setProperty("slang", PlayerSettings.slang) }
        function onAlangChanged() { mpvController.setProperty("alang", PlayerSettings.alang) }
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
            chapters: mpvController.chapterList

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
            onOpenRecentRequested: (path) => mpvController.openFile(path)
            onToggleFullscreen: window.toggleFullscreen()
            onTogglePip: window.togglePip()
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

        // Skip Intro Floating Button (Filmin / Netflix style)
        CachyButton {
            id: skipIntroBtn
            anchors.right: parent.right
            anchors.bottom: controlBar.top
            anchors.rightMargin: 24
            anchors.bottomMargin: 12
            z: 25
            visible: currentIntroEnd > 0 && mpvController.position >= currentIntroStart && mpvController.position < currentIntroEnd
            text: qsTr("Saltar introducción ▶")
            iconName: "media-seek-forward"
            onClicked: {
                mpvController.position = currentIntroEnd + 0.5
                hudPill.show("media-seek-forward", qsTr("Introducción saltada"))
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
        function onVideoSizeChanged() {
            if (PlayerSettings.autoFitWindow && !isFullscreen && !isPip && mpvController.hasVideo && mpvController.videoWidth > 0 && mpvController.videoHeight > 0) {
                var aspect = mpvController.videoAspect > 0 ? mpvController.videoAspect : (mpvController.videoWidth / mpvController.videoHeight)
                var currentW = window.width
                var calculatedH = Math.round(currentW / aspect)
                if (calculatedH >= 340 && calculatedH <= 1200) {
                    window.height = calculatedH
                }
            }
        }
    }

    function togglePip() {
        if (isFullscreen) {
            window.showNormal()
            isFullscreen = false
        }

        if (isPip) {
            isPip = false
            window.width = savedGeometry.width
            window.height = savedGeometry.height
            window.x = savedGeometry.x
            window.y = savedGeometry.y
            hudPill.show("picture-in-picture", qsTr("Modo estándar"))
        } else {
            savedGeometry = { width: window.width, height: window.height, x: window.x, y: window.y }
            isPip = true
            var pipW = 420
            var pipAspect = mpvController.videoAspect > 0 ? mpvController.videoAspect : (16.0 / 9.0)
            window.width = pipW
            window.height = Math.round(pipW / pipAspect)
            hudPill.show("picture-in-picture", qsTr("Picture in Picture"))
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
