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

using namespace bb::device;

FolderWatcher::FolderWatcher(QObject *_parent) :
    QObject(_parent),
    fileSystemWatcher(new QFileSystemWatcher(this)),
    sdCardInfo(new bb::device::SdCardInfo(this))
{
    Settings* settings = Settings::instance();
    QStringList folders = settings->value(SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY, "").toStringList();
    this->addFolders(folders);

    connect(fileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onDirectoryChanged(const QString&)));
    connect(sdCardInfo, SIGNAL(stateChanged(bb::device::SdCardState::Type)), this, SLOT(onSdCardStateChanged(bb::device::SdCardState::Type)));

    QStringList defaultFoldersList = this->getDefaultDeviceFolders();
    if (!this->isWatchingDefaultFolders(defaultFoldersList)) {
        this->addFoldersAndSubfolders(defaultFoldersList);
    }

    defaultFoldersList = this->getDefaultSdFolders();
    if (!this->isWatchingDefaultFolders(defaultFoldersList)) {
        this->addFoldersAndSubfolders(defaultFoldersList);
    }
}

bool FolderWatcher::isWatchingDefaultFolders(QStringList defaultFoldersList) {
    QStringList folders = this->getFolders();
    if (defaultFoldersList.isEmpty() || folders.contains(defaultFoldersList.first())) {
        return true;
    }
    return false;
}

void FolderWatcher::onSdCardStateChanged(SdCardState::Type sdCardState) {
    LOG(QString("FolderWatcher::onSdCardStateChanged(): " + STRING(sdCardState)));

    if (sdCardState != SdCardState::Mounted) {
        return;
    }

    QStringList folders = this->getFolders();
    QStringList defaultSdFolders = this->getDefaultSdFolders();
    if (defaultSdFolders.isEmpty() || folders.contains(defaultSdFolders.first())) {
        // Already watching SD card
        return;
    }

    this->addFoldersAndSubfolders(this->getDefaultSdFolders());
}

void FolderWatcher::onDirectoryChanged(const QString& folderPath) {
    LOG(QString("FolderWatcher::onDirectoryChanged(): " + folderPath));

    QFileInfo defaultFileInfo;
    QFileInfo lastEntry = this->getLastEntry(folderPath, defaultFileInfo);

    if (lastEntry == defaultFileInfo) {
        return;
    }

    if (lastEntry.isDir()) {
        this->addFolderAndSubfolders(lastEntry.absoluteFilePath());
        return;
    }

    QString newFilePath = lastEntry.absoluteFilePath();
    if (newFilePath.isNull() || newFilePath.isEmpty() || !this->isFileWithoutExtension(newFilePath)) {
        return;
    }

    if (ImageFileSignatureChecker::isAnImage(newFilePath)) {
        emit imageWithoutExtensionFound(newFilePath);
    }
}

QFileInfo FolderWatcher::getLastEntry(QString folderPath, QFileInfo defaultFileInfo) {
    QDir dir(folderPath);
    QDir::Filters filters = QDir::Files | QDir::Hidden;
    QDir::SortFlags flags = QDir::Time;
    QFileInfoList fileInfoList = dir.entryInfoList(filters, flags);

    if (fileInfoList.isEmpty()) {
        return defaultFileInfo;
    }

    return fileInfoList.first();
}

bool FolderWatcher::isFileWithoutExtension(QString filePath) {
    QString fileName = filePath.split("/").last();
    return !fileName.contains(".");
}

QStringList FolderWatcher::getDefaultDeviceFolders() {
    QStringList folders;
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/photos/");
    return folders;
}

QStringList FolderWatcher::getDefaultSdFolders() {
    QStringList folders;
    folders.append(QString(getenv("PERIMETER_HOME")) + "/removable/sdcard/photos/");
    return folders;
}

void FolderWatcher::addFolder(QString folder) {
    if (fileSystemWatcher->directories().contains(folder))
        return;

    fileSystemWatcher->addPath(folder);
    this->saveFolders();
}

void FolderWatcher::addFolders(QStringList folders) {
    fileSystemWatcher->addPaths(folders);
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

void FolderWatcher::addFoldersAndSubfolders(QStringList folders) {
    foreach(QString folder, folders) {
        this->addFolderAndSubfolders(folder);
    }
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
