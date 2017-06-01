/*
 * ImageFileSignatureChecker.h
 *
 *  Created on: May 31, 2017
 *      Author: roger
 */

#ifndef IMAGEFILESIGNATURECHECKER_H_
#define IMAGEFILESIGNATURECHECKER_H_

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QImage>

namespace ImageFileType {
    enum ImageFileType {
        BMP, // 42 4D
        ICO, // 00 00 01 00
        JPG, // FF D8 FF DB  OR  FF D8 FF E0 nn nn 4A 46 49 46 00 01  OR  FF D8 FF E1 nn nn 45 78 69 66 00 00
        GIF, // 47 49 46 38 37 61   OR  47 49 46 38 39 61
        PNG, // 89 50 4E 47 0D 0A 1A 0A
        TIFF // 49 49 2A 00    OR   4D 4D 00 2A
    };
}

class ImageFileSignatureChecker : public QObject
{
    Q_OBJECT

public:
    ImageFileSignatureChecker(QString filePath, QObject* parent = NULL);

    static bool isAnImage(QString filePath);

    ImageFileType::ImageFileType getImageFileType();
    QString setImageExtension(ImageFileType::ImageFileType imageFileType);

private:
    QString filePath;
};

#endif /* IMAGEFILESIGNATURECHECKER_H_ */
