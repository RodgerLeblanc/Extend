/*
 * Settings.h
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "common.hpp"
#include "helpers.hpp"

#include <src/HeadlessCommunication/HeadlessCommunication.h>

#include <QObject>
#include <bb/system/ApplicationStartupMode>
#include <bb/data/DataAccessError>
#include <bb/data/JsonDataAccess>
#include <QStringList>

#include <QMutex>
#include <QMutexLocker>
#include <QtNetwork/QUdpSocket>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap allSettings READ allSettings NOTIFY allSettingsChanged);

public:
    static Settings* instance() {
        return Settings::instance(NULL, NULL);
    }

    static Settings *instance(HeadlessCommunication* headlessCommunication, QObject* parent = NULL) {
        static Settings *instance;

        if (!instance)
            instance = new Settings(headlessCommunication, parent);

        Q_ASSERT(instance->settingsCommunication != NULL);

        return instance;
    }

    Q_INVOKABLE QStringList allKeys();
    Q_INVOKABLE bool contains(const QString &key);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QString fileName();
    Q_INVOKABLE int remove(const QString &key);
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

    QVariantMap getSettingsMap() const { return settings; }

public slots:
    Q_INVOKABLE void sync();

private slots:
    void onReceivedData(QString reason, QVariant data);

private:
    QVariantMap allSettings() { return settings; }
    void save();

    HeadlessCommunication* settingsCommunication;

    Environment::Type environment;
    bool isInUiThread;
    bb::data::JsonDataAccess jda;
    QMutex mutex;
    QVariantMap settings;


protected:
    Settings(HeadlessCommunication* headlessCommunication, QObject* parent = NULL);
    virtual ~Settings();

signals:
    void allSettingsChanged(const QVariantMap&);
};

#endif /* SETTINGS_H_ */

