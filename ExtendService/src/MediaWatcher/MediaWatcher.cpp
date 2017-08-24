/*
 * MediaWatcher.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: roger
 */

#include <src/MediaWatcher/MediaWatcher.h>
#include <src/common.hpp>
#include <src/Logger/Logger.h>
#include <src/ImageFileSignatureChecker/ImageFileSignatureChecker.h>

MediaWatcher::MediaWatcher(QObject *_parent) :
    QObject(_parent),
    ppsWatch(new PpsWatch("/pps/services/multimedia/sync/changes", this))
{
    connect(ppsWatch, SIGNAL(ppsFileReady(const QVariantMap&)), this, SLOT(onPpsFileReady(const QVariantMap&)));
}

MediaWatcher::~MediaWatcher()
{
}

void MediaWatcher::onPpsFileReady(const QVariantMap& map) {
    //LOG("MediaWatcher::onPpsFileReady():", bb::PpsObject::encode(map));

    if (map[PPS_KEY_CHANGE].toString().compare(QString("added"), Qt::CaseInsensitive) != 0) {
        return;
    }

    MediaFile mediaFile;
    if (map.contains(PPS_KEY_FID)) { mediaFile.fileId = map[PPS_KEY_FID].toLongLong(); }
    if (map.contains(PPS_KEY_FOLDERID)) { mediaFile.folderId = map[PPS_KEY_FOLDERID].toLongLong(); }
    if (map.contains(PPS_KEY_FTYPE)) { mediaFile.fileType = fileTypeFromValue(map[PPS_KEY_FTYPE].toInt()); }
    if (map.contains(PPS_KEY_PATH)) { mediaFile.path = normalizePath(map[PPS_KEY_PATH].toString()); }
    if (map.contains(PPS_KEY_PERIMETER)) { mediaFile.perimeter = filePerimeterFromValue(map[PPS_KEY_PERIMETER].toString()); }

    if (!mediaFile.isValid()) {
        return;
    }

    QString path = mediaFile.getFullPath();

    if (!ImageFileSignatureChecker::isFileWithoutExtension(path)) {
        return;
    }

    if (ImageFileSignatureChecker::isNoMediaFolder(path.left(path.lastIndexOf("/")))) {
        return;
    }

    LOG("map:", bb::PpsObject::encode(map));
    emit imageWithoutExtensionFound(path);
}

FilePerimeter::Type MediaWatcher::filePerimeterFromValue(QString filePerimeterValue) {
    if (filePerimeterValue.compare(QString("personal"), Qt::CaseInsensitive) == 0) {
        return FilePerimeter::Personal;
    }
    else if (filePerimeterValue.compare(QString("enterprise"), Qt::CaseInsensitive) == 0) {
        return FilePerimeter::Enterprise;
    }
    else if (filePerimeterValue.compare(QString("sdcard"), Qt::CaseInsensitive) == 0) {
        return FilePerimeter::SDCard;
    }

    return FilePerimeter::Unknown;
}

FileType::Type MediaWatcher::fileTypeFromValue(int fileTypeValue)
{
    switch(fileTypeValue) {
    case 1:
        return FileType::Audio;
    case 2:
        return FileType::Video;
    case 3:
        return FileType::AudioVideo;
    case 4:
        return FileType::Photo;
    case 5:
        return FileType::Device;
    case 6:
        return FileType::Document;
    case 99:
        return FileType::Other;
    default:
        return FileType::Unknown;
    }
}

QString MediaWatcher::normalizePath(QString path) {
    if (path.startsWith("/")) {
        path.remove(0, 1);
    }

    return path;
}
