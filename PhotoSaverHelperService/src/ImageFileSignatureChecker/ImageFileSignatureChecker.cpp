/*
 * ImageFileSignatureChecker.cpp
 *
 *  Created on: May 31, 2017
 *      Author: roger
 */

#include "ImageFileSignatureChecker.h"
#include <src/Logger/Logger.h>

#include <bb/data/JsonDataAccess>
#include <QStringList>

ImageFileSignatureChecker::ImageFileSignatureChecker(QString filePath, QObject* parent) :
    QObject(parent),
    filePath(filePath)
{
}

bool ImageFileSignatureChecker::isAnImage(QString filePath) {
    QImage image(filePath);
    return !image.isNull();
}

ImageFileExtensionType ImageFileSignatureChecker::getImageFileType() {
    bb::data::JsonDataAccess jda;

    QString thisSig = this->getFileSignature();
    QVariantMap imageFileTypeSignaturesMap = jda.load(IMAGE_FILE_TYPE_SIGNATURE_FILE).toMap();

    foreach(QString key, imageFileTypeSignaturesMap.keys()) {
        QStringList sigsForThisExtension = imageFileTypeSignaturesMap[key].toStringList();
        foreach(QString sig, sigsForThisExtension) {
            int maxSigSize = qMin(thisSig.size(), sig.size());
            if (thisSig.left(maxSigSize) == sig.left(maxSigSize)) {
                return this->getImageFileTypeByName(key);
            }
        }
    }

    LOG("Image file type can't be retrieved");
    return UNKNOWN;
}

ImageFileExtensionType ImageFileSignatureChecker::getImageFileTypeByName(QString name) {
    for(int i = FIRST; i <= LAST; i++) {
        ImageFileExtensionType thisType = (ImageFileExtensionType)i;
        if (this->getImageFileTypeName(thisType) == name) {
            return thisType;
        }
    }
    LOG("Image file type can't be retrieved");
    return UNKNOWN;
}

QString ImageFileSignatureChecker::getFileSignature() {
    QString sig;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        sig = file.read(16);
        file.flush();
        file.close();
    }
    file.deleteLater();
    return sig;
}

QString ImageFileSignatureChecker::getImageFileTypeName(ImageFileExtensionType imageFileType) {
    switch (imageFileType) {
        case UNKNOWN :
            return "";
        case BMP :
            return "BMP";
        case ICO :
            return "ICO";
        case JPG :
            return "JPG";
        case GIF :
            return "GIF";
        case PNG :
            return "PNG";
        case TIFF :
            return "TIFF";
    }
    return "";
}

QString ImageFileSignatureChecker::setImageExtension(ImageFileExtensionType imageFileType) {
    QString extension = this->getImageFileTypeName(imageFileType).toLower();
    if (extension.isEmpty()) {
        emit this->error(filePath, UnspecifiedError, "Can't find a matching extension");
        return filePath;
    }

    QFile file(filePath);
    QString newFilePath = filePath + "." + extension;
    while (QFile::exists(newFilePath)) {
        newFilePath.insert(newFilePath.lastIndexOf("."), "_copy");
    }

    bool ok = file.rename(newFilePath);

    if (!ok) {
        emit this->error(filePath, (ImageFileSignatureCheckerError)file.error(), file.errorString());
    }

    return ok ? newFilePath : filePath;
}
