/*
 * FolderWatcher.h
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#ifndef FOLDERWATCHER_H_
#define FOLDERWATCHER_H_

#define SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY          "SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY"

#include <QObject>
#include <QFileSystemWatcher>

class FolderWatcher : public QObject
{
    Q_OBJECT

public:
    FolderWatcher(QObject *_parent = NULL);

    void addFolder(QString folder);
    void addFolderAndSubfolders(QString folder);
    void addFolders(QStringList folders);
    void removeFolder(QString folder);
    void removeFolders(QStringList folders);
    QStringList getFolders();

private slots:
    void onDirectoryChanged(const QString&);

private:
    QString getNewFileFromFolder(QString folderPath);
    bool isFileWithoutExtension(QString filePath);
    void saveFolders();

    QFileSystemWatcher* fileSystemWatcher;

signals:
    void imageWithoutExtensionFound(const QString&);
};

#endif /* FOLDERWATCHER_H_ */
