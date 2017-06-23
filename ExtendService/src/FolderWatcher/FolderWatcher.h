/*
 * FolderWatcher.h
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#ifndef FOLDERWATCHER_H_
#define FOLDERWATCHER_H_

#include <QObject>
#include <QFileSystemWatcher>
#include <bb/device/SdCardInfo>

class FolderWatcher : public QObject
{
    Q_OBJECT

public:
    FolderWatcher(QObject *_parent = NULL);
    virtual ~FolderWatcher();

    void addFolder(QString folder);
    void addFolders(QStringList folders);
    void addFolderAndSubfolders(QString folder);
    void addFoldersAndSubfolders(QStringList folders);
    void removeFolder(QString folder);
    void removeFolders(QStringList folders);
    QStringList getFolders();

public slots:
    void init();

private slots:
    void onDirectoryChanged(const QString&);
    void onSdCardStateChanged(bb::device::SdCardState::Type);

private:
    bool alreadyWatching(QStringList defaultFolders);
    void cleanFolder(QString folderPath);
    QStringList getDefaultDeviceFolders();
    QStringList getDefaultSdFolders();
    QFileInfo& getLastEntry(QString folderPath, QFileInfo& defaultFileInfo);
    bool isFileWithoutExtension(QString filePath);
    bool isWatchingDefaultFolders(QStringList defaultFoldersList);
    void saveFolders();
    bool shouldBeWatched(QString folder);

    QFileSystemWatcher* fileSystemWatcher;
    bb::device::SdCardInfo* sdCardInfo;

signals:
    void imageWithoutExtensionFound(const QString&);
};

#endif /* FOLDERWATCHER_H_ */
