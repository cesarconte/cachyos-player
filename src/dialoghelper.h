#pragma once

#include <QObject>
#include <QUrl>
#include <QStringList>

class NativeDialogHelper : public QObject
{
    Q_OBJECT

public:
    explicit NativeDialogHelper(QObject *parent = nullptr);

    Q_INVOKABLE void openFolder(const QString &title = QString());
    Q_INVOKABLE void openFiles(const QString &title = QString(), const QString &filter = QString());
    Q_INVOKABLE void openSingleFile(const QString &title = QString(), const QString &filter = QString());

Q_SIGNALS:
    void folderSelected(const QString &folderPath);
    void filesSelected(const QStringList &filePaths);
    void fileSelected(const QString &filePath);

private:
    void callPortalOpenFile(const QString &title, bool directory, bool multiple);
};
