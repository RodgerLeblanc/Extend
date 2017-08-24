/*
 * MediaWatcher.h
 *
 *  Created on: Jul 4, 2017
 *      Author: roger
 */

#ifndef MEDIAWATCHER_H_
#define MEDIAWATCHER_H_

#define PPS_KEY_CHANGE          "change"
#define PPS_KEY_FID             "fid"
#define PPS_KEY_FOLDERID        "folderid"
#define PPS_KEY_FTYPE           "ftype"
#define PPS_KEY_PATH            "path"
#define PPS_KEY_PERIMETER       "perimeter"

#include "src/Enums/FilePerimeter.h"
#include "src/Enums/FileType.h"
#include "src/MediaFile/MediaFile.h"
#include "src/PpsWatch/PpsWatch.h"

#include <QObject>
#include <bb/device/SdCardInfo>

class MediaWatcher : public QObject
{
    Q_OBJECT

public:
    MediaWatcher(QObject *_parent = NULL);
    virtual ~MediaWatcher();

private slots:
    void onPpsFileReady(const QVariantMap&);

private:
    FilePerimeter::Type filePerimeterFromValue(QString filePerimeterValue);
    FileType::Type fileTypeFromValue(int fileTypeValue);
    QString normalizePath(QString path);

    PpsWatch* ppsWatch;

signals:
    void imageWithoutExtensionFound(const QString&);
};

#endif /* MEDIAWATCHER_H_ */
