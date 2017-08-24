/*
 * HeadlessCommunication.h
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 *      Contributor: Roger Leblanc
 */

#ifndef HEADLESSCOMMUNICATION_H_
#define HEADLESSCOMMUNICATION_H_

#include "common.hpp"
#include "helpers.hpp"

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <bb/system/InvokeRequest>

class HeadlessCommunication : public QObject
{
	Q_OBJECT
public:
    HeadlessCommunication(Environment::Type environmentType, QObject* parent);
	virtual ~HeadlessCommunication() {}

    Q_INVOKABLE void sendMessage(QString message);
    Q_INVOKABLE void sendMessage(const QString& reason, const QString& message);
    Q_INVOKABLE void sendMessage(const QString& reason, const QVariant& variant);

    Environment::Type getEnvironment();

private slots:
    void onInvoked(const bb::system::InvokeRequest&);
    void onReadyRead();

private:
    QUdpSocket* socket;

	Environment::Type environmentType;
	bool isInUiThread;
	int listeningPort, sendingPort;

signals:
    void receivedData(QString);
    void receivedData(QString reason, QVariant data);
};

#endif /* HEADLESSCOMMUNICATION_H_ */
