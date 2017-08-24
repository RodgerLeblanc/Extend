/*
 * Logger.cpp
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

#include <src/Logger/Logger.h>

#include <QDebug>
#include <QFile>

#include <bb/device/HardwareInfo>
#include <bb/platform/Notification>
#include <bb/platform/PlatformInfo>

using namespace bb::data;
using namespace bb::device;
using namespace bb::platform;

Logger::Logger(QObject* parent) :
    QObject(parent),
    lastSaveDateTime(QDateTime::currentDateTime())
{
}

Logger::~Logger()
{
    save();
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

QVariantMap Logger::getLog() {
    HardwareInfo hardwareInfo;
    PlatformInfo platformInfo;

    save();
    QVariantMap log = Logger::instance()->logMap;
    log.insert("App version", QCoreApplication::applicationVersion());
    log.insert("Device", QString(hardwareInfo.modelName() + " " + platformInfo.osVersion()));
    return log;
}

Logger* Logger::init(HeadlessCommunication* headlessCommunication, QObject* parent) {
    Q_ASSERT(headlessCommunication->getEnvironment() != Environment::Unknown);

    Logger* instance = Logger::instance(parent);

    instance->loggerCommunication = headlessCommunication;
    instance->setEnvironment(headlessCommunication->getEnvironment());

    if (!instance->isInUiThread) {
        connect(instance->loggerCommunication, SIGNAL(receivedData(QString, QVariant)), instance, SLOT(onReceivedData(QString, QVariant)));
        instance->sync();
    }

    return instance;
}

void Logger::logFinal(QString message) {
    Logger* logger = Logger::instance();
    if (logger->isInUiThread) {
        logger->loggerCommunication->sendMessage(LOGGER_LOG_THIS, message);
        return;
    }

    qDebug() << message;

    QStringList log = logger->logMap["log"].toStringList();
    log.prepend(QTime::currentTime().toString(TIME_TO_STRING) + " " + QString::number(HeapUsage::measureMem()) + " " + message);

    while (log.size() > MAXIMUM_LOG_SIZE) {
        log.removeLast();
    }

    logger->logMap.insert("log", log);

    QDateTime now = QDateTime::currentDateTime();
    if (logger->lastSaveDateTime.secsTo(now) > 60) {
        save();
    }
}

void Logger::onReceivedData(QString reason, QVariant data) {
    if (reason == LOGGER_LOG_THIS) {
        QString message = data.toString();
        log(message);
    }
    else if (reason == HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION) {
        save();
        loggerCommunication->sendMessage(LOG_READY_FOR_BUG_REPORT);
    }
    else if (reason == HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION) {
        QString title = APP_NAME;
        QString body = STRING(Logger::getLog());
        Notification notification;
        notification.setTitle(title);
        notification.setBody(body);
        notification.notify();
    }
}

void Logger::save()
{
    if (Logger::instance()->isInUiThread)
        return;

    Helpers::safeWriteJsonFile(Logger::instance()->logMap, LOG_FILE);
}

void Logger::setEnvironment(Environment::Type environmentType) {
    environment = environmentType;
    isInUiThread = environment == Environment::UI;
    qDebug() << "Logger::setEnvironment()" << environment << isInUiThread;
}

void Logger::sync() {
    logMap = Helpers::safeReadJsonFile(LOG_FILE).toMap();
}
