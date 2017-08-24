/*
 * Settings.cpp
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

#include <src/Settings/Settings.h>

#include <QDebug>
#include <QFile>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>

Settings::Settings(HeadlessCommunication* headlessCommunication, QObject* parent) :
    QObject(parent),
    settingsCommunication(headlessCommunication),
    environment(headlessCommunication->getEnvironment()),
    isInUiThread(headlessCommunication->getEnvironment() == Environment::UI)
{
    // Load the settings file on startup.
    sync();

    connect(settingsCommunication, SIGNAL(receivedData(QString, QVariant)), this, SLOT(onReceivedData(QString, QVariant)));
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

    // Only headless is in charge of saving.
    if (isInUiThread) {
        settingsCommunication->sendMessage(SETTINGS_CLEAR);
    }
    else {
        save();
    }
}

bool Settings::contains(const QString &key)
{
    // Returns true if the settings file contains an item with key key;
    // otherwise returns false.
    return settings.contains(key);
}

QString Settings::fileName()
{
    return SETTINGS_FILE;
}

void Settings::onReceivedData(QString reason, QVariant data) {
    if (reason == SETTINGS_FILE_UPDATED) {
        sync();
    }

    // Only headless should perform the next if (reason == xxx)
    if (isInUiThread)
        return;

    if (reason == SETTINGS_SAVE_THIS) {
        QVariantMap map = data.toMap();
        foreach(QString key, map.keys()) {
            this->setValue(key, map[key]);
        }
    }
    else if (reason == SETTINGS_CLEAR) {
        this->clear();
    }
    else if (reason == SETTINGS_REMOVE_KEY) {
        this->remove(data.toString());
    }
}

int Settings::remove(const QString &key)
{
    // Removes all the items that have the key key from the settings file.
    // Returns the number of items removed which is usually 1 but will be 0
    // if the key isn't in the settings file.

    int numberRemoved = settings.remove(key);

    // Only headless is in charge of saving.
    if (isInUiThread) {
        settingsCommunication->sendMessage(SETTINGS_REMOVE_KEY, key);
    }
    else {
        save();
    }

    return numberRemoved;
}

void Settings::save()
{
    if (isInUiThread) {
        return;
    }

    Helpers::safeWriteJsonFile(settings, SETTINGS_FILE);

    emit allSettingsChanged(settings);
    settingsCommunication->sendMessage(SETTINGS_FILE_UPDATED);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    // Inserts a new item with the key key and a value of value.
    // If there is already an item with the key key, that item's value is
    // replaced with value.

    settings.insert(key, value);

    // Only headless is in charge of saving.
    if (isInUiThread) {
        QVariantMap map;
        map.insert(key, value);
        settingsCommunication->sendMessage(SETTINGS_SAVE_THIS, map);
    }
    else {
        save();
    }
}

void Settings::sync()
{
    settings = Helpers::safeReadJsonFile(SETTINGS_FILE).toMap();

    emit allSettingsChanged(settings);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue)
{
    // Returns the value associated with the key key.
    // If the settings file contains no item with key key,
    // the function returns defaultValue.

    // Return on empty key.
    if (key.trimmed().isEmpty())
        return QVariant();

    if (settings.contains(key))
        return settings.value(key, defaultValue);
    else
        return defaultValue;
}
