/*
 * ImageFileSignatureChecker.h
 *
 *  Created on: May 31, 2017
 *      Author: roger
 */

#ifndef IMAGEFILESIGNATURECHECKER_H_
#define IMAGEFILESIGNATURECHECKER_H_

#define IMAGE_FILE_TYPE_SIGNATURE_FILE "app/native/assets/imageFileTypeSignatures.json"

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QImage>

namespace ImageFileExtension {
    enum Type {
        UNKNOWN = -1,
        BMP = 0,
        ICO = 1,
        JPG = 2,
        GIF = 3,
        PNG = 4,
        TIFF = 5,

        FIRST = BMP,
        LAST = TIFF
    };
}

class ImageFileSignatureChecker : public QObject
{
    Q_OBJECT

public:
    ImageFileSignatureChecker(QString filePath, QObject* parent = NULL);

    static bool isAnImage(QString filePath);

    ImageFileExtension::Type getImageFileType();
    QString setImageExtension(ImageFileExtension::Type imageFileType);

private:
    QString getImageFileTypeName(ImageFileExtension::Type imageFileType);
    ImageFileExtension::Type getImageFileTypeByName(QString name);
    QString getFileSignature();

    QString filePath;
};

#endif /* IMAGEFILESIGNATURECHECKER_H_ */
