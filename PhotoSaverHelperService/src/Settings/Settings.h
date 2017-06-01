/*
 * Settings.h
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <src/common.hpp>

#include <QObject>
#include <bb/data/JsonDataAccess>

#include <QMutex>
#include <QMutexLocker>
#include <QtNetwork/QUdpSocket>
#include <QStringList>

#define SETTINGS_FILE "data/settings.json"

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap allSettings READ allSettings NOTIFY allSettingsChanged);

public:
    static Settings *instance(QObject *parent = NULL) {
        static Settings *instance;

        if (!instance)
            instance = new Settings(parent);

        return instance;
    }

    Q_INVOKABLE QStringList allKeys();
    Q_INVOKABLE bool contains(QString key);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QString fileName();
    Q_INVOKABLE int remove(QString key);
    Q_INVOKABLE void setValue(QString key, QVariant value);
    Q_INVOKABLE void sync();
    Q_INVOKABLE QVariant value(QString key, QVariant defaultValue = QVariant());

    QVariantMap getSettingsMap() const { return settings; }

private slots:
    void onReadyRead();

private:
    QVariantMap allSettings() { return settings; }
    void handleMessage(QString data);
    void listenOnPort(int port);
    void save();
    void sendMessage(QString data);
    void sendObject(QVariantMap object);

    QUdpSocket *m_socket;
    QUdpSocket *m_server;

    QVariantMap settings;
    QMutex mutex;

protected:
    Settings(QObject *parent);
    virtual ~Settings();

signals:
    void allSettingsChanged(const QVariantMap&);
};

#endif /* SETTINGS_H_ */

