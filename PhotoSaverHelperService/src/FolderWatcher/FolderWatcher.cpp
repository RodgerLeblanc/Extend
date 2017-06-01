/*
 * FolderWatcher.cpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#include "FolderWatcher.h"
#include "src/Settings/Settings.h"

#include <QStringList>

namespace FolderWatcher
{

    FolderWatcher::FolderWatcher(QObject *_parent) :
        QObject(_parent),
        m_fileSystemWatcher(new QFileSystemWatcher(this))
    {
        Settings* settings = Settings::instance();
        QStringList folders = settings->value(SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY, "").toStringList();
        this->addFolders(folders);
    }

    void FolderWatcher::addFolder(QString folder) {
        m_fileSystemWatcher->addPath(folder);
        this->saveFolders();
    }

    void FolderWatcher::addFolders(QStringList folders) {
        m_fileSystemWatcher->addPaths(folders);
        this->saveFolders();
    }

    void FolderWatcher::removeFolder(QString folder) {
        m_fileSystemWatcher->removePath(folder);
        this->saveFolders();
    }

    void FolderWatcher::removeFolders(QStringList folders) {
        m_fileSystemWatcher->removePaths(folders);
        this->saveFolders();
    }

    QStringList FolderWatcher::getFolders() {
        return m_fileSystemWatcher->directories();
    }

    void FolderWatcher::saveFolders() {
        Settings* settings = Settings::instance();
        settings->setValue(SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY, this->getFolders());
    }
} /* namespace FolderWatcher */
