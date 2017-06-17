/*
 * Settings.cpp
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

/*
 *
 * Don't forget to add this to your PRO file :
 * LIBS += -lbbdata
 *
 * To add this object to your project, follow these steps :
 * 1- Copy both Settings.cpp and Settings.h to your src folder
 * 2- In your applicationui.hpp or service.hpp, add those lines :
 *      #include "Settings.h"
 *
 *      private:
 *          Settings *settings;
 *
 * 3- In your application.cpp or service.cpp, add those lines :
 *      (in constructor)
 *      settings = new Settings(this);
 *
 */

#include <src/Settings/Settings.h>
#include <QDebug>
#include <QFile>

using namespace bb::data;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    // If the settings file doesn't exists, create it
    QFile jsonFile(SETTINGS_FILE);
    if (!jsonFile.exists())
        save();

    jsonFile.deleteLater();

    // Load the settings file on startup.
    sync();

    m_server = new QUdpSocket(this);
    m_socket = new QUdpSocket(this);

    this->listenOnPort(SETTINGS_HEADLESS_LISTENING_PORT);
}

Settings::~Settings()
{
    // Save the settings file on exit.
    save();
}

QStringList Settings::allKeys()
{
    // Returns a list containing all the keys in the map in ascending order.
    return settings.keys();
}

void Settings::clear()
{
    // Removes all items from the settings file.
    settings.clear();
    save();
}

bool Settings::contains(QString key)
{
    // Returns true if the settings file contains an item with key key;
    // otherwise returns false.
    return settings.contains(key);
}

QString Settings::fileName()
{
    return SETTINGS_FILE;
}

void Settings::handleMessage(QString data) {
    JsonDataAccess jda;
    QVariantMap message = jda.loadFromBuffer(data).toMap();

    if (message.contains(REQUEST_KEY)) {
        QString request = message[REQUEST_KEY].toString();
        QString key = message[REQUEST_KEY_KEY].toString();

        if (request == REQUEST_SET_SETTINGS) {
            QVariant value = message[REQUEST_VALUE_KEY];
            this->setValue(key, value);

            this->sendObject(message);
        }
        else if (request == REQUEST_GET_SETTINGS) {
            QVariant defaultValue = message[REQUEST_DEFAULT_VALUE_KEY];
            QVariant value = this->value(key, defaultValue);

            message.insert(REQUEST_VALUE_KEY, value);
            this->sendObject(message);
        }
        else if (request == REQUEST_GET_ALL_SETTINGS) {
            message.insert(REQUEST_ALL_SETTINGS_KEY, this->allSettings());
            this->sendObject(message);
        }
    }
}

void Settings::listenOnPort(int port)
{
    qDebug() << "Settings::listenOnPort()" << port;
    m_server->bind(QHostAddress::Any, port);
    bool ok = connect(m_server, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    if (ok)
        qDebug() << "binding ok";
    else
        qDebug() << "binding failed";
}

void Settings::onReadyRead()
{
  qDebug() << "Settings::onReadyRead()";
    while (m_server->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_server->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_server->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        QString data = QString(datagram);

//        qDebug() << data;
//        emit receivedData(data);

        handleMessage(data);
    }
}

int Settings::remove(QString key)
{
    // Removes all the items that have the key key from the settings file.
    // Returns the number of items removed which is usually 1 but will be 0
    // if the key isn't in the settings file.

    int numberRemoved = settings.remove(key);
    save();
    return numberRemoved;
}

void Settings::save()
{
    QMutexLocker locker(&mutex);

    JsonDataAccess jda;
    jda.save(QVariant(settings), SETTINGS_FILE);

    locker.unlock();

    emit allSettingsChanged(settings);
}

void Settings::sendMessage(QString data)
{
    m_socket->writeDatagram(data.toStdString().c_str(),QHostAddress("127.0.0.1"), SETTINGS_UI_LISTENING_PORT);
}

void Settings::sendObject(QVariantMap object) {
    QString data;
    JsonDataAccess jda;
    jda.saveToBuffer(object, &data);
    this->sendMessage(data);
}

void Settings::setValue(QString key, QVariant value)
{
    // Inserts a new item with the key key and a value of value.
    // If there is already an item with the key key, that item's value is
    // replaced with value.

//    qDebug() << "HL - Settings::setValue ->" << key << ":" << value;

    settings.remove(key);
    settings.insert(key, value);
    save();
}

void Settings::sync()
{
    QMutexLocker locker(&mutex);

    // Reloads any settings that have been changed by another application.
    // This function is called automatically by the event loop at regular intervals,
    // so you normally don't need to call it yourself.
    JsonDataAccess jda;
    settings = jda.load(SETTINGS_FILE).toMap();

    locker.unlock();

    emit allSettingsChanged(settings);
}

QVariant Settings::value(QString key, QVariant defaultValue)
{
    // Returns the value associated with the key key.
    // If the settings file contains no item with key key,
    // the function returns defaultValue.

    // Return on empty key.
    if (key.trimmed().isEmpty())
        return QVariant();

    if (settings.contains(key)) {
        return settings.value(key, defaultValue);
    }
    else
        return defaultValue;
}
