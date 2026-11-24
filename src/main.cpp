#include <clocale>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QQuickWindow>
#include <QQuickStyle>

#include "mpvcontroller.h"
#include "mpvitem.h"
#include "mprismanager.h"
#include "settingsmanager.h"
#include "subsyncmanager.h"
#include "dialoghelper.h"

int main(int argc, char *argv[])
{
    // libmpv strictly requires LC_NUMERIC to be C
    std::setlocale(LC_NUMERIC, "C");

    // Force native Wayland backend with real alpha buffer for Liquid Glass blur
    if (qgetenv("QT_QPA_PLATFORM").isEmpty()) {
        qputenv("QT_QPA_PLATFORM", "wayland;xcb");
    }
    QQuickWindow::setDefaultAlphaBuffer(true);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    // High-performance Wayland attributes
    QGuiApplication::setOrganizationName(QStringLiteral("CachyOS"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("cachyos.org"));
    QGuiApplication::setApplicationName(QStringLiteral("cachyos-player"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("CachyOS Player"));

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("multimedia-video-player"), QIcon(QStringLiteral(":/icons/cachyos-player.png"))));

    // Register QML types
    qmlRegisterType<MpvItem>("org.cachyos.player", 1, 0, "MpvItem");
    qmlRegisterType<MpvController>("org.cachyos.player", 1, 0, "MpvController");
    qmlRegisterType<MprisManager>("org.cachyos.player", 1, 0, "MprisManager");
    qmlRegisterType<NativeDialogHelper>("org.cachyos.player", 1, 0, "NativeDialogHelper");
    qmlRegisterSingletonType<SettingsManager>("org.cachyos.player", 1, 0, "PlayerSettings",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine);
            Q_UNUSED(scriptEngine);
            return new SettingsManager();
        });
    qmlRegisterSingletonType<SubSyncManager>("org.cachyos.player", 1, 0, "SubSync",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine);
            Q_UNUSED(scriptEngine);
            return new SubSyncManager();
        });

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("/usr/lib/qt6/qml"));

    // Pass CLI arguments if a video file is given
    QString initialFile;
    if (argc > 1) {
        initialFile = QString::fromLocal8Bit(argv[1]);
    }
    engine.rootContext()->setContextProperty(QStringLiteral("initialFileUrl"), initialFile);

    const QUrl url(QStringLiteral("qrc:/org/cachyos/player/qml/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
