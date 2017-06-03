/*
 * ImageFileSignatureChecker.cpp
 *
 *  Created on: May 31, 2017
 *      Author: roger
 */

#include "ImageFileSignatureChecker.h"

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

ImageFileExtension::Type ImageFileSignatureChecker::getImageFileType() {
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

    return ImageFileExtension::UNKNOWN;
}

ImageFileExtension::Type ImageFileSignatureChecker::getImageFileTypeByName(QString name) {
    for(int i = ImageFileExtension::FIRST; i <= ImageFileExtension::LAST; i++) {
        ImageFileExtension::Type thisType = (ImageFileExtension::Type)i;
        if (this->getImageFileTypeName(thisType) == name) {
            return thisType;
        }
    }
    return ImageFileExtension::UNKNOWN;
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

QString ImageFileSignatureChecker::getImageFileTypeName(ImageFileExtension::Type imageFileType) {
    switch (imageFileType) {
        case ImageFileExtension::UNKNOWN :
            return "";
        case ImageFileExtension::BMP :
            return "BMP";
        case ImageFileExtension::ICO :
            return "ICO";
        case ImageFileExtension::JPG :
            return "JPG";
        case ImageFileExtension::GIF :
            return "GIF";
        case ImageFileExtension::PNG :
            return "PNG";
        case ImageFileExtension::TIFF :
            return "TIFF";
    }
    return "";
}

QString ImageFileSignatureChecker::setImageExtension(ImageFileExtension::Type imageFileType) {
    QString extension = this->getImageFileTypeName(imageFileType).toLower();
    if (extension.isEmpty())
        return filePath;

    QString newFilePath = filePath + "." + extension;
    bool ok = QFile::rename(filePath, newFilePath);
    Q_ASSERT(ok);

    return ok ? newFilePath : filePath;
}
