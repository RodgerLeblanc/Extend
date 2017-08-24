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
#include <QVariantMap>

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
    ImageFileSignatureChecker(QObject* parent = NULL);

    static bool isAnImage(QString filePath);
    static bool isFileWithoutExtension(QString filePath);
    static bool isNoMediaFolder(QString folderPath);

    QString setImageExtension(QString filePath);

private:
    QString getImageFileExtension(QString filePath);
    QVariantMap getSignaturesMap();
    QByteArray getFileSignature(QString filePath);
    static bool isAnImageNotHandledByQImage(QString filePath);
    bool renameFile(QString filePath, QString newFilePath);
    bool signaturesMatch(QString sig1, QString sig2);

    QVariantMap imageFileTypeSignaturesMap;

signals:
    void error(QString filePath, ImageFileSignatureCheckerError error, QString errorMessage);
};

#endif /* IMAGEFILESIGNATURECHECKER_H_ */
