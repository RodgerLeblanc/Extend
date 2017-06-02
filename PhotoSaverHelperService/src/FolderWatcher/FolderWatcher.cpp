/*
 * FolderWatcher.cpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#include "FolderWatcher.h"
#include <src/Logger/Logger.h>
#include "src/Settings/Settings.h"
#include "src/ImageFileSignatureChecker/ImageFileSignatureChecker.h"

#include <QStringList>
#include <QDir>
#include <QDateTime>

FolderWatcher::FolderWatcher(QObject *_parent) :
    QObject(_parent),
    fileSystemWatcher(new QFileSystemWatcher(this))
{
    Settings* settings = Settings::instance();
    QStringList folders = settings->value(SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY, "").toStringList();
    this->addFolders(folders);

    connect(fileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onDirectoryChanged(const QString&)));
}

void FolderWatcher::onDirectoryChanged(const QString& folderPath) {
    QString newFilePath = this->getNewFileFromFolder(folderPath);
    if (newFilePath.isNull() || newFilePath.isEmpty())
        return;

    if (!this->isFileWithoutExtension(newFilePath))
        return;

    bool isAnImage = ImageFileSignatureChecker::isAnImage(newFilePath);

    if (isAnImage)
        emit imageWithoutExtensionFound(newFilePath);
}

QString FolderWatcher::getNewFileFromFolder(QString folderPath) {
    QDir dir(folderPath);
    QDir::Filters filters = QDir::Files | QDir::Hidden;
    QDir::SortFlags flags = QDir::Time;
    QFileInfoList fileInfoList = dir.entryInfoList(filters, flags);

    if (fileInfoList.isEmpty())
        return "";

    QFileInfo mostRecentFileInfo = fileInfoList.first();
    QDateTime created = mostRecentFileInfo.created();
    QDateTime lastModified = mostRecentFileInfo.lastModified();
    QDateTime now = QDateTime::currentDateTime();
//    if ((created.secsTo(now) > 5) && (lastModified.secsTo(now) > 5))
//        return "";

    return mostRecentFileInfo.absoluteFilePath();
}

bool FolderWatcher::isFileWithoutExtension(QString filePath) {
    QString fileName = filePath.split("/").last();
    return !fileName.contains(".");
}

void FolderWatcher::addFolder(QString folder) {
    if (fileSystemWatcher->directories().contains(folder))
        return;

    fileSystemWatcher->addPath(folder);
    this->saveFolders();
}

void FolderWatcher::addFolderAndSubfolders(QString folder) {
    this->addFolder(folder);

    QDir dir(folder);
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot;
    QFileInfoList fileInfoList = dir.entryInfoList(filters);

    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.isDir()) {
            this->addFolderAndSubfolders(fileInfo.absoluteFilePath());
        }
    }
}

void FolderWatcher::addFolders(QStringList folders) {
    fileSystemWatcher->addPaths(folders);
    this->saveFolders();
}

void FolderWatcher::removeFolder(QString folder) {
    fileSystemWatcher->removePath(folder);
    this->saveFolders();
}

void FolderWatcher::removeFolders(QStringList folders) {
    fileSystemWatcher->removePaths(folders);
    this->saveFolders();
}

QStringList FolderWatcher::getFolders() {
    return fileSystemWatcher->directories();
}

void FolderWatcher::saveFolders() {
    Settings* settings = Settings::instance();
    QStringList folders = this->getFolders();
    folders.removeDuplicates();
    settings->setValue(SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY, folders);
}
