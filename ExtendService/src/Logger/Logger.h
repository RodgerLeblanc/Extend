/*
 * Logger.h
 *
 *  Created on: 2015-06-08
 *      Author: Roger
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define LOG(args...) Logger::log(args)
#define LOG_VAR(x) Logger::log(QString(QString(#x) + ": " + Logger::convertToString(x)))
#define STRING(x) Logger::convertToString(x)

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

    static QVariantMap getLog();
    static void save();

    template<typename T>
    static void log(T t) {
        Logger::addToOutput(t);
        Logger::logFinal(Logger::instance()->output);
        Logger::instance()->output = "";
    }

    template<typename T, typename... Args>
    static void log(T t, Args... args) {
        Logger::addToOutput(t);
        log(args...);
    }

    static QString convertToString(QVariant variant);

private:
    template<typename T>
    static void addToOutput(T t) {
        if (!Logger::instance()->output.isEmpty()) { Logger::instance()->output += " "; }
        Logger::instance()->output += Logger::convertToString(t);
    }

    static void logFinal(QString message);

    QString output;
    QVariantMap logMap;
    QDateTime lastSaveDateTime;

protected:
    Logger(QObject* parent);
    virtual ~Logger();
};

#endif /* LOGGER_H_ */
