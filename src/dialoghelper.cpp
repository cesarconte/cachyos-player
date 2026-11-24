#include "dialoghelper.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDebug>
#include <QUrl>

class PortalResponseWatcher : public QObject
{
    Q_OBJECT
public:
    PortalResponseWatcher(NativeDialogHelper *helper, const QString &path, bool isDir, bool isSingleFile = false, QObject *parent = nullptr)
        : QObject(parent), m_helper(helper), m_path(path), m_isDir(isDir), m_isSingleFile(isSingleFile) {}

public Q_SLOTS:
    void onResponse(uint response, const QVariantMap &results) {
        if (response == 0 && m_helper) {
            QStringList uris = results.value(QStringLiteral("uris")).toStringList();
            if (m_isDir) {
                if (!uris.isEmpty()) {
                    QString dirUri = uris.first();
                    QString localPath = dirUri.startsWith(QStringLiteral("file://")) ? QUrl(dirUri).toLocalFile() : dirUri;
                    Q_EMIT m_helper->folderSelected(localPath);
                }
            } else if (m_isSingleFile) {
                if (!uris.isEmpty()) {
                    QString fileUri = uris.first();
                    QString localPath = fileUri.startsWith(QStringLiteral("file://")) ? QUrl(fileUri).toLocalFile() : fileUri;
                    Q_EMIT m_helper->fileSelected(localPath);
                }
            } else {
                QStringList paths;
                for (const auto &u : uris) {
                    paths << (u.startsWith(QStringLiteral("file://")) ? QUrl(u).toLocalFile() : u);
                }
                if (!paths.isEmpty()) {
                    Q_EMIT m_helper->filesSelected(paths);
                }
            }
        }
        deleteLater();
    }

private:
    NativeDialogHelper *m_helper;
    QString m_path;
    bool m_isDir;
    bool m_isSingleFile;
};

NativeDialogHelper::NativeDialogHelper(QObject *parent)
    : QObject(parent)
{
}

void NativeDialogHelper::openFolder(const QString &title)
{
    callPortalOpenFile(title.isEmpty() ? QStringLiteral("Abrir carpeta / temporada") : title, true, false);
}

void NativeDialogHelper::openFiles(const QString &title, const QString &/*filter*/)
{
    callPortalOpenFile(title.isEmpty() ? QStringLiteral("Abrir vídeo(s) o audio(s)") : title, false, true);
}

void NativeDialogHelper::openSingleFile(const QString &title, const QString &/*filter*/)
{
    callPortalOpenFile(title.isEmpty() ? QStringLiteral("Seleccionar archivo") : title, false, false);
}

void NativeDialogHelper::callPortalOpenFile(const QString &title, bool directory, bool multiple)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.FileChooser"),
        QStringLiteral("OpenFile")
    );

    QVariantMap options;
    if (directory) {
        options.insert(QStringLiteral("directory"), true);
    }
    if (multiple) {
        options.insert(QStringLiteral("multiple"), true);
    }
    options.insert(QStringLiteral("modal"), true);

    msg << QString() << title << options;

    QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() != QDBusMessage::ReplyMessage) {
        qWarning() << "NativeDialogHelper: Portal call failed:" << reply.errorMessage();
        return;
    }

    QDBusObjectPath handle = reply.arguments().at(0).value<QDBusObjectPath>();
    QString requestPath = handle.path();

    bool isSingle = !directory && !multiple;
    auto *watcher = new PortalResponseWatcher(this, requestPath, directory, isSingle);
    QDBusConnection::sessionBus().connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        requestPath,
        QStringLiteral("org.freedesktop.portal.Request"),
        QStringLiteral("Response"),
        watcher,
        SLOT(onResponse(uint,QVariantMap))
    );
}

#include "dialoghelper.moc"
