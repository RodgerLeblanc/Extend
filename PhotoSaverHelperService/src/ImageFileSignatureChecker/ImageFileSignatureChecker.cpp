/*
 * ImageFileSignatureChecker.cpp
 *
 *  Created on: May 31, 2017
 *      Author: roger
 */

#include "ImageFileSignatureChecker.h"

ImageFileSignatureChecker::ImageFileSignatureChecker(QString filePath, QObject* parent) :
    QObject(parent),
    filePath(filePath)
{
}

bool ImageFileSignatureChecker::isAnImage(QString filePath) {
    QImage image(filePath);
    return !image.isNull();
}

ImageFileType::ImageFileType ImageFileSignatureChecker::getImageFileType() {
    QString content;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        content = file.read(20);
        file.flush();
        file.close();
    }
    file.deleteLater();

    return ImageFileType::JPG;
}

QString ImageFileSignatureChecker::setImageExtension(ImageFileType::ImageFileType imageFileType) {
    QString extension;
    switch (imageFileType) {
        case ImageFileType::BMP :
            extension = ".bmp";
            break;
        case ImageFileType::ICO :
            extension = ".ico";
            break;
        case ImageFileType::JPG :
            extension = ".jpg";
            break;
        case ImageFileType::GIF :
            extension = ".gif";
            break;
        case ImageFileType::PNG :
            extension = ".png";
            break;
        case ImageFileType::TIFF :
            extension = ".tiff";
            break;
    }

    QString newFilePath = filePath + extension;
    bool ok = QFile::rename(filePath, newFilePath);
    Q_ASSERT(ok);

    return newFilePath;
}
