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

ImageFileSignatureChecker::ImageFileSignatureChecker(QObject* parent) :
    QObject(parent)
{
    imageFileTypeSignaturesMap = this->getSignaturesMap();
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

bool ImageFileSignatureChecker::isFileWithoutExtension(QString filePath) {
    QString fileName = filePath.split("/").last();
    return !fileName.contains(".");
}

bool ImageFileSignatureChecker::isNoMediaFolder(QString folderPath) {
    QDir dir(folderPath);
    QStringList nameFilters = QStringList() << "*.nomedia";
    QDir::Filters filters = QDir::Hidden | QDir::AllEntries | QDir::NoDotAndDotDot;
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, filters);

    return !fileInfoList.isEmpty();
}

QString ImageFileSignatureChecker::setImageExtension(QString filePath) {
    QString extension = this->getImageFileExtension(filePath);
    if (extension.isEmpty()) {
        emit this->error(filePath, UnspecifiedError, "Can't find a matching extension");
        return filePath;
    }

    QString newFilePath = filePath + "." + extension.toLower();
    bool ok = renameFile(filePath, newFilePath);

    return ok ? newFilePath : filePath;
}

QString ImageFileSignatureChecker::getImageFileExtension(QString filePath) {
    QByteArray unknownSig = this->getFileSignature(filePath);

    foreach(QString key, imageFileTypeSignaturesMap.keys()) {
        QStringList sigsForThisExtension = imageFileTypeSignaturesMap[key].toStringList();
        foreach(QString knownSig, sigsForThisExtension) {
            if (signaturesMatch(unknownSig, knownSig)) {
                return key;
            }
        }
    }

    return QString();
}

QVariantMap ImageFileSignatureChecker::getSignaturesMap() {
    bb::data::JsonDataAccess jda;
    return jda.load(IMAGE_FILE_TYPE_SIGNATURE_FILE).toMap();
}

QByteArray ImageFileSignatureChecker::getFileSignature(QString filePath) {
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

bool ImageFileSignatureChecker::signaturesMatch(QString unknownSig, QString knownSig) {
    unknownSig.remove(" ");
    knownSig.remove(" ");

    unknownSig = unknownSig.left(knownSig.size());

    return unknownSig.contains(QRegExp(knownSig, Qt::CaseInsensitive));
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
