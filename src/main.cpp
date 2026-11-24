#include <clocale>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QQuickWindow>

#include "mpvcontroller.h"
#include "mpvitem.h"
#include "settingsmanager.h"
#include "subsyncmanager.h"

int main(int argc, char *argv[])
{
    // libmpv strictly requires LC_NUMERIC to be C
    std::setlocale(LC_NUMERIC, "C");

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
