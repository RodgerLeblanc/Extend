/*
 * HeadlessCommunication.cpp
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 */

#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Logger/Logger.h>
#include "common.hpp"

#include <bb/PpsObject>

HeadlessCommunication::HeadlessCommunication(QObject *parent) :
    QObject(parent)
{
	m_server = new QUdpSocket(this);
	m_socket = new QUdpSocket(this);

	listenOnPort(UI_LISTENING_PORT);
}

void HeadlessCommunication::listenOnPort(int port)
{
    qDebug() << "HeadlessCommunication::listenOnPort()" << port;
	m_server->bind(QHostAddress::Any, port);
	bool ok = connect(m_server, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	ok==true ? qDebug() << "binding ok" : qDebug() << "binding failed";
}

void HeadlessCommunication::sendMessage(QString message)
{
//  qDebug() << "HeadlessCommunication::sendMessage()" << message;
    m_socket->writeDatagram(message.toStdString().c_str(),QHostAddress("127.0.0.1"), HEADLESS_LISTENING_PORT);
}

void HeadlessCommunication::sendMessage(const QString& reason, const QVariantMap& data) {
    QVariantMap map;
    map.insert("reason", reason);
    map.insert("data", data);
    this->sendMessage(bb::PpsObject::encode(map));
}

void HeadlessCommunication::sendToWatchface(QString message)
{
    qDebug() << "HeadlessCommunication::sendToWatchface()" << message;
    m_socket->writeDatagram(message.toStdString().c_str(),QHostAddress("127.0.0.1"), 9877);
}

void HeadlessCommunication::onReadyRead()
{
//	qDebug() << "HeadlessCommunication::dataReceived";
    while (m_server->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_server->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_server->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        QString data = QString(datagram);

//        qDebug() << data;
        emit receivedData(data);
    }
}
