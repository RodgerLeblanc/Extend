/*
 * FolderCleaner.h
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#ifndef FOLDERCLEANER_H_
#define FOLDERCLEANER_H_

#include <QObject>
#include <bb/device/SdCardInfo>

class FolderCleaner : public QObject
{
    Q_OBJECT

public:
    FolderCleaner(QObject *_parent = NULL);
    virtual ~FolderCleaner();

    void cleanFolders(QStringList folders);
    void cleanFolderAndSubfolders(QString folder);
    void cleanFoldersAndSubfolders(QStringList folders);

public slots:
    void init();

private slots:
    void onSdCardStateChanged(bb::device::SdCardState::Type);

private:
    void cleanFolder(QString folderPath);
    QStringList getDefaultDeviceFolders();
    QStringList getDefaultSdFolders();
    bool shouldBeCleaned(QString folder);

    bb::device::SdCardInfo* sdCardInfo;

signals:
    void imageWithoutExtensionFound(const QString&);
    void initDone();
};

#endif /* FOLDERCLEANER_H_ */
