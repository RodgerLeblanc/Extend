/*
 * Logger.h
 *
 *  Created on: 2015-06-08
 *      Author: Roger
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define STRING(x) Logger::convertToString(x)
#define LOG(x) Logger::log(Logger::convertToString(x))
#define LOG_VAR(x) Logger::log(QString(QString(#x) + ": " + Logger::convertToString(x)))

#include <src/Logger/HeapUsage/HeapUsage.h>

#include <bb/data/JsonDataAccess>
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>

#define LOG_FILE "data/log.txt"
#define MAXIMUM_LOG_SIZE 100

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger* instance(QObject* parent = NULL) {
        static Logger* instance;

        if (!instance)
            instance = new Logger(parent);

        return instance;
    }

    static void log(QString message);
    static QString convertToString(QVariant variant);
    static QVariantMap getLog();
    static void save();

private:

    QVariantMap logMap;
    QDateTime lastSaveDateTime;

protected:
    Logger(QObject* parent);
    virtual ~Logger();
};

#endif /* LOGGER_H_ */
