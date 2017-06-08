/*
 * HeadlessCommunication.h
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 */

#ifndef HEADLESSCOMMUNICATION_H_
#define HEADLESSCOMMUNICATION_H_

#include <QObject>
#include <QVariantMap>
#include <QtNetwork/QUdpSocket>

class HeadlessCommunication : public QObject
{
	Q_OBJECT
public:
	HeadlessCommunication(QObject *parent = NULL);
	virtual ~HeadlessCommunication() {}

	// RX
	void listenOnPort(int port);

	// TX
    Q_INVOKABLE void sendMessage(QString message);
    Q_INVOKABLE void sendMessage(const QString& reason, const QVariantMap& data);
    Q_INVOKABLE void sendToWatchface(QString message);

private:
	QUdpSocket *m_socket;
	QUdpSocket *m_server;

signals:
	void receivedData(QString);

public slots:
	void onReadyRead();
};

#endif /* HEADLESSCOMMUNICATION_H_ */
