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
 * 3- Use it anywhere in your cpp file using the macro, it accepts
 * any number of arguments of QVariant type :
 *      LOG("Hello World!", 123, true, QDateTime::currentDateTime());
 *
 * There's also two other macros available :
 *      LOG_VAR(x)  ===> will log {variable name}: {variable value}
 *      STRING(x)   ===> will convert any QVariant to QString
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

void Logger::logFinal(QString message) {
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
