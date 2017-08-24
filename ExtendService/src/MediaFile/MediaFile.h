/*
 * MediaFile.h
 *
 *  Created on: Jul 4, 2017
 *      Author: roger
 */

#ifndef MEDIAFILE_H_
#define MEDIAFILE_H_

#define PERSONAL_PERIMETER_PATH         QString("/accounts/1000/shared/")
#define ENTERPRISE_PERIMETER_PATH       QString("/accounts/1000-enterprise/shared/")
#define SDCARD_PERIMETER_PATH           QString("/accounts/1000/removable/sdcard/")

#include "src/Enums/FilePerimeter.h"
#include "src/Enums/FileType.h"

class MediaFile
{
public:
    MediaFile() { alreadyVerified = false; }

    bool isValid() { return path != "" &&
                            fileId >= 0 &&
                            folderId >= 0 &&
                            perimeter != FilePerimeter::Unknown; }

    QString getFullPath() {
        switch(perimeter) {
            case FilePerimeter::Personal: {
                return PERSONAL_PERIMETER_PATH + path;
            }
            case FilePerimeter::Enterprise: {
                return ENTERPRISE_PERIMETER_PATH + path;
            }
            case FilePerimeter::SDCard: {
                return SDCARD_PERIMETER_PATH + path;
            }
            default: {
                return PERSONAL_PERIMETER_PATH + path;
            }
        }
    }

    QString path;
    qlonglong fileId;
    qlonglong folderId;
    FileType::Type fileType;
    FilePerimeter::Type perimeter;
    bool alreadyVerified;
};

#endif /* MEDIAFILE_H_ */
