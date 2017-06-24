/*
 * FolderWatcher.cpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#include "FolderWatcher.h"

#include <src/common.hpp>
#include <src/Logger/Logger.h>
#include <src/Settings/Settings.h>
#include <src/ImageFileSignatureChecker/ImageFileSignatureChecker.h>

#include <QStringList>
#include <QDir>
#include <QDateTime>
#include <pthread.h>

using namespace bb::device;

FolderWatcher::FolderWatcher(QObject *_parent) :
    QObject(_parent),
    fileSystemWatcher(new QFileSystemWatcher(this)),
    sdCardInfo(new bb::device::SdCardInfo(this))
{
    connect(fileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onDirectoryChanged(const QString&)));
    connect(sdCardInfo, SIGNAL(stateChanged(bb::device::SdCardState::Type)), this, SLOT(onSdCardStateChanged(bb::device::SdCardState::Type)));
}

FolderWatcher::~FolderWatcher() {}

void FolderWatcher::init() {
    QDateTime now = QDateTime::currentDateTime();
    LOG("FolderWatcher::init() started on thread", pthread_self());
    this->addFoldersAndSubfolders(this->getDefaultDeviceFolders());
    this->addFoldersAndSubfolders(this->getDefaultSdFolders());
    LOG("FolderWatcher::init() done in", now.secsTo(QDateTime::currentDateTime()), "seconds.");
}

bool FolderWatcher::isWatchingDefaultFolders(QStringList defaultFoldersList) {
    QStringList folders = this->getFolders();
    if (defaultFoldersList.isEmpty() || folders.contains(defaultFoldersList.first())) {
        return true;
    }
    return false;
}

void FolderWatcher::onSdCardStateChanged(SdCardState::Type sdCardState) {
    LOG("FolderWatcher::onSdCardStateChanged():", STRING(sdCardState));

    switch (sdCardState) {
        case SdCardState::Mounted: {
            this->addFoldersAndSubfolders(this->getDefaultSdFolders());
            break;
        }
        case SdCardState::Unmounted: {
            QString sdRoot = QString(getenv("PERIMETER_HOME")) + "/removable/sdcard/";
            QStringList sdFolders = this->getFolders();
            sdFolders.filter(sdRoot, Qt::CaseInsensitive);
            this->removeFolders(sdFolders);
            break;
        }
        case SdCardState::Busy:
        case SdCardState::Unknown:
        default: {
            break;
        }
    }
}

bool FolderWatcher::alreadyWatching(QStringList defaultFolders) {
    return defaultFolders.isEmpty() || this->getFolders().contains(defaultFolders.first());
}

void FolderWatcher::onDirectoryChanged(const QString& folderPath) {
    LOG("FolderWatcher::onDirectoryChanged():", folderPath);

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

    bool isAnImage = ImageFileSignatureChecker::isAnImage(newFilePath);
    LOG(newFilePath, "isAnImage?", isAnImage);
    if (isAnImage) {
        emit imageWithoutExtensionFound(newFilePath);
    }
}

QFileInfo& FolderWatcher::getLastEntry(QString folderPath, QFileInfo& defaultFileInfo) {
    QDir dir(folderPath);
    QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks;
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
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/camera");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/documents");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/downloads");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/misc");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/photos");
    return folders;
}

QStringList FolderWatcher::getDefaultSdFolders() {
    QStringList folders;
    folders.append(QString(getenv("PERIMETER_HOME")) + "/removable/sdcard/");
    return folders;
}

void FolderWatcher::addFolder(QString folder) {
    if (fileSystemWatcher->directories().contains(folder))
        return;

    if (!shouldBeWatched(folder)) {
        return;
    }

    this->cleanFolder(folder);
    fileSystemWatcher->addPath(folder);
}

void FolderWatcher::addFolders(QStringList folders) {
    fileSystemWatcher->addPaths(folders);
}

void FolderWatcher::addFolderAndSubfolders(QString folder) {
    if (!shouldBeWatched(folder)) {
        return;
    }

    this->addFolder(folder);

    QDir dir(folder);
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks;
    QFileInfoList fileInfoList = dir.entryInfoList(filters);

    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.isDir()) {
            this->addFolderAndSubfolders(fileInfo.absoluteFilePath());
        }
    }
}

bool FolderWatcher::shouldBeWatched(QString folder) {
    QFileInfo fileInfo(folder);
    if (fileInfo.isSymLink()) {
        return false;
    }

    QString boxFolderPath = QString(getenv("PERIMETER_HOME")) + "/shared/Box";
    QString dropboxFolderPath = QString(getenv("PERIMETER_HOME")) + "/shared/Dropbox";
    QString androidFolderPath = QString(getenv("PERIMETER_HOME")) + "/shared/misc/android";
    if (folder.contains(boxFolderPath, Qt::CaseInsensitive) ||
            folder.contains(dropboxFolderPath, Qt::CaseInsensitive) ||
            folder.contains(androidFolderPath, Qt::CaseInsensitive)) {
        return false;
    }

    return true;
}

void FolderWatcher::addFoldersAndSubfolders(QStringList folders) {
    foreach(QString folder, folders) {
        this->addFolderAndSubfolders(folder);
    }
}

void FolderWatcher::removeFolder(QString folder) {
    fileSystemWatcher->removePath(folder);
}

void FolderWatcher::removeFolders(QStringList folders) {
    fileSystemWatcher->removePaths(folders);
}

QStringList FolderWatcher::getFolders() {
    return fileSystemWatcher->directories();
}

void FolderWatcher::cleanFolder(QString folderPath) {
    QDir dir(folderPath);
    QDir::Filters filters = QDir::Files;
    QFileInfoList fileInfoList = dir.entryInfoList(filters);

    foreach(QFileInfo fileInfo, fileInfoList) {
        QString newFilePath = fileInfo.absoluteFilePath();
        if (this->isFileWithoutExtension(newFilePath)) {
            if (ImageFileSignatureChecker::isAnImage(newFilePath)) {
                emit imageWithoutExtensionFound(newFilePath);
            }
        }
    }
}
