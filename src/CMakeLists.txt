set(SOURCES
    main.cpp
    mpvcontroller.h
    mpvcontroller.cpp
    mpvitem.h
    mpvitem.cpp
    settingsmanager.h
    settingsmanager.cpp
)

qt_add_executable(cachyos-player ${SOURCES})

target_link_libraries(cachyos-player PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::OpenGL
    PkgConfig::MPV
    CachyOSUI::cachyosui
)

target_include_directories(cachyos-player PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_qml_module(cachyos-player
    URI org.cachyos.player.qml
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
        qml/Main.qml
        qml/PreferencesDialog.qml
        qml/ControlBar.qml
        qml/MediaButton.qml
        qml/TrackMenu.qml
)

install(TARGETS cachyos-player
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES resources/cachyos-player.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)
