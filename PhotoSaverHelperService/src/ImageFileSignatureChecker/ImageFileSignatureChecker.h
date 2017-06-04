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

typedef enum ImageFileExtensionType {
    UNKNOWN = -1,
    BMP = 0,
    ICO = 1,
    JPG = 2,
    GIF = 3,
    PNG = 4,
    TIFF = 5,

    FIRST = BMP,
    LAST = TIFF
} ImageFileExtensionType;

typedef enum ImageFileSignatureCheckerError {
    NoError = QFile::NoError,
    ReadError = QFile::ReadError,
    WriteError = QFile::WriteError,
    FatalError = QFile::FatalError,
    ResourceError = QFile::ResourceError,
    OpenError = QFile::OpenError,
    AbortError = QFile::AbortError,
    TimeOutError = QFile::TimeOutError,
    UnspecifiedError = QFile::UnspecifiedError,
    RemoveError = QFile::RemoveError,
    RenameError = QFile::RenameError,
    PositionError = QFile::PositionError,
    ResizeError = QFile::ResizeError,
    PermissionsError = QFile::PermissionsError,
    CopyError = QFile::CopyError
} ImageFileSignatureCheckerError;

class ImageFileSignatureChecker : public QObject
{
    Q_OBJECT

public:
    ImageFileSignatureChecker(QString filePath, QObject* parent = NULL);

    static bool isAnImage(QString filePath);

    ImageFileExtensionType getImageFileType();
    QString setImageExtension(ImageFileExtensionType imageFileType);

private:
    QString getImageFileTypeName(ImageFileExtensionType imageFileType);
    ImageFileExtensionType getImageFileTypeByName(QString name);
    QString getFileSignature();

    QString filePath;

signals:
    void error(QString filePath, ImageFileSignatureCheckerError error, QString errorMessage);
};

#endif /* IMAGEFILESIGNATURECHECKER_H_ */
