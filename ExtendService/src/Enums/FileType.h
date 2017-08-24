/*
 * FileType.h
 *
 *  Created on: Jul 4, 2017
 *      Author: roger
 */

#ifndef FILETYPE_H_
#define FILETYPE_H_

#include <QObject>

class FileType
{
    Q_GADGET
    Q_ENUMS(Type)
public:
    enum Type {
        Unknown = 0,
        Audio = 1,
        Video = 2,
        AudioVideo = 3,
        Photo = 4,
        Device = 5,
        Document = 6,
        Other = 99
    };
private:
    FileType() {}
    ~FileType() {}
};

#endif /* FILETYPE_H_ */
