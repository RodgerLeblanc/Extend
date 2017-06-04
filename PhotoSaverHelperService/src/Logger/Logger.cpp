/*
 * Logger.cpp
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

/*
 *
 * To add this object to your project, follow these steps :
 * 1- Copy Logger folder to your src folder
 * 2- In your applicationui.hpp or service.hpp, add those lines :
 *      #include <src/Logger/Logger.h>
 *
 *      private:
 *          Logger* logger;
 *
 * 3- In your application.cpp or service.cpp, add those lines :
 *      (in constructor)
 *      logger = Logger::instance(this);
 *
 * 4- Use it anywhere in your cpp file :
 *      logger.log("Hello World!";
 *
 */

#include <src/Logger/Logger.h>
#include <QDebug>
#include <QFile>

#include <bb/device/HardwareInfo>
#include <bb/platform/PlatformInfo>

using namespace bb::data;

Logger::Logger(QObject *parent) :
    QObject(parent),
    lastSaveDateTime(QDateTime::currentDateTime())
{
    JsonDataAccess jda;
    logMap = jda.load(LOG_FILE).toMap();
}

Logger::~Logger()
{
    log("~Logger()");
    save();
}

QVariantMap Logger::getLog() {
    bb::device::HardwareInfo hardwareInfo;
    bb::platform::PlatformInfo platformInfo;

    save();
    QVariantMap log = Logger::instance()->logMap;
    log.insert("App version", QCoreApplication::applicationVersion());
    log.insert("Device", QString(hardwareInfo.modelName() + " " + platformInfo.osVersion()));
    return log;
}

void Logger::log(QString message) {
    qDebug() << message;

    Logger* logger = Logger::instance();

    QStringList log = logger->logMap["log"].toStringList();
    log.prepend(QTime::currentTime().toString("hh:mm:ss") + " " + QString::number(HeapUsage::measureMem()) + " " + message);

    while (log.size() > MAXIMUM_LOG_SIZE) {
        log.removeLast();
    }

    logger->logMap.insert("log", log);

    QDateTime now = QDateTime::currentDateTime();
    if (logger->lastSaveDateTime.secsTo(now) > 60) {
        save();
    }
}

void Logger::save()
{
    JsonDataAccess jda;
    QMutex mutex;
    QMutexLocker locker(&mutex);
    jda.save(Logger::instance()->logMap, LOG_FILE);
    locker.unlock();
}

QString Logger::convertToString(QVariant variant) {
    if (variant.canConvert(QVariant::String)) {
        return variant.toString();
    }

    QString toReturn;
    JsonDataAccess jda;
    jda.saveToBuffer(variant, &toReturn);

    return toReturn;
}
