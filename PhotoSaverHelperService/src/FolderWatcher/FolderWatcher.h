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

namespace FolderWatcher
{

    class FolderWatcher : public QObject
    {
        Q_OBJECT

    public:
        FolderWatcher(QObject *_parent = NULL);

        void addFolder(QString folder);
        void addFolders(QStringList folders);
        void removeFolder(QString folder);
        void removeFolders(QStringList folders);
        QStringList getFolders();

    private:
        void saveFolders();

        QFileSystemWatcher* m_fileSystemWatcher;
    };

} /* namespace FolderWatcher */

#endif /* FOLDERWATCHER_H_ */
