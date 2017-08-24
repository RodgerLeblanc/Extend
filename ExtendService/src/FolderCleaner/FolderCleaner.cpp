/*
 * FolderCleaner.cpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#include "FolderCleaner.h"

#include <src/common.hpp>
#include <src/Logger/Logger.h>
#include <src/Settings/Settings.h>
#include <src/ImageFileSignatureChecker/ImageFileSignatureChecker.h>

#include <QStringList>
#include <QDir>
#include <QDateTime>

using namespace bb::device;

FolderCleaner::FolderCleaner(QObject *_parent) :
    QObject(_parent),
    sdCardInfo(new bb::device::SdCardInfo(this))
{
    connect(sdCardInfo, SIGNAL(stateChanged(bb::device::SdCardState::Type)), this, SLOT(onSdCardStateChanged(bb::device::SdCardState::Type)));
}

FolderCleaner::~FolderCleaner() {}

void FolderCleaner::init() {
    QDateTime now = QDateTime::currentDateTime();
    LOG("FolderCleaner::init() started on thread", (int)QThread::currentThreadId());

    this->cleanFoldersAndSubfolders(this->getDefaultDeviceFolders());
    this->cleanFoldersAndSubfolders(this->getDefaultSdFolders());

    LOG("FolderCleaner::init() done in", now.secsTo(QDateTime::currentDateTime()), "seconds.");
    emit this->initDone();
}

void FolderCleaner::cleanFoldersAndSubfolders(QStringList folders) {
    foreach(QString folder, folders) {
        this->cleanFolderAndSubfolders(folder);
    }
}

void FolderCleaner::cleanFolderAndSubfolders(QString folder) {
    if (!shouldBeCleaned(folder)) {
        return;
    }

    this->cleanFolder(folder);

    QDir dir(folder);
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks;
    QFileInfoList fileInfoList = dir.entryInfoList(filters);

    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.isDir()) {
            this->cleanFolderAndSubfolders(fileInfo.absoluteFilePath());
        }
    }
}

void FolderCleaner::cleanFolder(QString folderPath) {
    QDir dir(folderPath);
    QDir::Filters filters = QDir::Files;
    QFileInfoList fileInfoList = dir.entryInfoList(filters);

    foreach(QFileInfo fileInfo, fileInfoList) {
        QString newFilePath = fileInfo.absoluteFilePath();
        if (ImageFileSignatureChecker::isFileWithoutExtension(newFilePath)) {
            emit imageWithoutExtensionFound(newFilePath);
        }
    }
}

void FolderCleaner::onSdCardStateChanged(SdCardState::Type sdCardState) {
    LOG("FolderCleaner::onSdCardStateChanged():", STRING(sdCardState));

    switch (sdCardState) {
        case SdCardState::Mounted: {
            this->cleanFoldersAndSubfolders(this->getDefaultSdFolders());
            return;
        }
        case SdCardState::Unmounted:
        case SdCardState::Busy:
        case SdCardState::Unknown:
        default: {
            return;
        }
    }
}

QStringList FolderCleaner::getDefaultDeviceFolders() {
    QStringList folders;
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/camera");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/documents");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/downloads");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/misc");
    folders.append(QString(getenv("PERIMETER_HOME")) + "/shared/photos");
    return folders;
}

QStringList FolderCleaner::getDefaultSdFolders() {
    QStringList folders;
    folders.append(QString(getenv("PERIMETER_HOME")) + "/removable/sdcard/");
    return folders;
}

bool FolderCleaner::shouldBeCleaned(QString folder) {
    QFileInfo fileInfo(folder);
    if (fileInfo.isSymLink()) {
        return false;
    }

    if (ImageFileSignatureChecker::isNoMediaFolder(folder)) {
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
