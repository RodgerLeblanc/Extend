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

#include "common.hpp"
#include "helpers.hpp"

#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Logger/HeapUsage/HeapUsage.h>

#include <bb/data/JsonDataAccess>
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>

#define MAXIMUM_LOG_SIZE 100

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger* init(HeadlessCommunication* headlessCommunication, QObject* parent = NULL);

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

//#if __cplusplus > 199711L
    template<typename T, typename... Args>
    static void log(T t, Args... args) {
        Logger::addToOutput(t);
        log(args...);
    }
//#endif

    static QString convertToString(QVariant variant);

private slots:
    void onReceivedData(QString reason, QVariant data);

private:
    template<typename T>
    static void addToOutput(T t) {
        if (!Logger::instance()->output.isEmpty()) { Logger::instance()->output += " "; }
        Logger::instance()->output += Logger::convertToString(t);
    }

    static void logFinal(QString message);

    void setCommunication();
    void setEnvironment(Environment::Type environmentType);
    void sync();

    HeadlessCommunication* loggerCommunication;

    Environment::Type environment;
    bool isInUiThread;
    QDateTime lastSaveDateTime;
    QVariantMap logMap;
    QString output;

protected:
    Logger(QObject* parent);
    virtual ~Logger();
};

#endif /* LOGGER_H_ */
