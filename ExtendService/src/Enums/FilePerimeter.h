/*
 * FilePerimeter.h
 *
 *  Created on: Jul 4, 2017
 *      Author: roger
 */

#ifndef FILEPERIMETER_H_
#define FILEPERIMETER_H_

#include <QObject>

class FilePerimeter
{
    Q_GADGET
    Q_ENUMS(Type)
public:
    enum Type {
        Unknown = 0,
        Personal = 1,
        Enterprise = 2,
        SDCard = 3
    };
private:
    FilePerimeter();
    ~FilePerimeter();
};

#endif /* FILEPERIMETER_H_ */
