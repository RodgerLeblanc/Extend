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
    return !image.isNull() || ImageFileSignatureChecker::isAnImageNotHandledByQImage(filePath);
}

bool ImageFileSignatureChecker::isAnImageNotHandledByQImage(QString filePath) {
    QByteArray sig;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        sig = file.read(16);
        file.flush();
        file.close();
    }
    file.deleteLater();
    sig = sig.toHex().toUpper();
    return sig.startsWith("FFD8FFED");
}

ImageFileExtensionType ImageFileSignatureChecker::getImageFileType() {
    bb::data::JsonDataAccess jda;

    QByteArray thisSig = this->getFileSignature();
    QVariantMap imageFileTypeSignaturesMap = jda.load(IMAGE_FILE_TYPE_SIGNATURE_FILE).toMap();

    foreach(QString key, imageFileTypeSignaturesMap.keys()) {
        QStringList sigsForThisExtension = imageFileTypeSignaturesMap[key].toStringList();
        foreach(QString sig, sigsForThisExtension) {
            if (signaturesMatch(thisSig, sig)) {
                return this->getImageFileTypeByName(key);
            }
        }
    }

    LOG("Image file type can't be retrieved");
    return UNKNOWN;
}

QByteArray ImageFileSignatureChecker::getFileSignature() {
    QByteArray sig;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        sig = file.read(16);
        file.flush();
        file.close();
    }
    file.deleteLater();
    return sig.toHex().toUpper();
}

bool ImageFileSignatureChecker::signaturesMatch(QString sig1, QString sig2) {
    sig1.remove(" ");
    sig2.remove(" ");
    int maxSigSize = qMin(sig1.size(), sig2.size());
    return sig1.left(maxSigSize) == sig2.left(maxSigSize);
}

ImageFileExtensionType ImageFileSignatureChecker::getImageFileTypeByName(QString name) {
    for(int i = FIRST_EXTENSION; i <= LAST_EXTENSION; i++) {
        ImageFileExtensionType thisType = (ImageFileExtensionType)i;
        if (this->getImageFileTypeName(thisType) == name) {
            return thisType;
        }
    }
    LOG("Image file type can't be retrieved");
    return UNKNOWN;
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

    QString newFilePath = filePath + "." + extension;
    bool ok = renameFile(filePath, newFilePath);

    return ok ? newFilePath : filePath;
}

bool ImageFileSignatureChecker::renameFile(QString filePath, QString newFilePath) {
    QFile file(filePath);
    while (QFile::exists(newFilePath)) {
        newFilePath.insert(newFilePath.lastIndexOf("."), "_copy");
    }

    bool ok = file.rename(newFilePath);
    if (!ok) {
        emit this->error(filePath, (ImageFileSignatureCheckerError)file.error(), file.errorString());
    }

    return ok;
}
