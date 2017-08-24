/*
 * HeadlessCommunication.cpp
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 *      Contributor: Roger Leblanc
 */

#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Logger/Logger.h>

#include <bb/PpsObject>

const QString reasonKey = HEADLESS_COMMUNICATION_REASON;
const QString dataKey = HEADLESS_COMMUNICATION_DATA;

HeadlessCommunication::HeadlessCommunication(Environment::Type environmentType, QObject* parent) :
    QObject(parent),
    socket(new QUdpSocket(this)),
    environmentType(environmentType),
    isInUiThread(environmentType == Environment::UI)
{
    listeningPort = isInUiThread ? HEADLESS_COMMUNICATION_PORT_UI : HEADLESS_COMMUNICATION_PORT_HL;
    sendingPort = isInUiThread ? HEADLESS_COMMUNICATION_PORT_HL : HEADLESS_COMMUNICATION_PORT_UI;

    socket->bind(QHostAddress::LocalHost, listeningPort);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

Environment::Type HeadlessCommunication::getEnvironment() {
    return this->environmentType;
}

void HeadlessCommunication::onInvoked(const bb::system::InvokeRequest& request) {
    QString reason = request.action();
    QVariant data = request.data();

    emit receivedData(reason, data);
}

void HeadlessCommunication::onReadyRead()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress senderHost;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &senderHost, &senderPort);

        bool ok = false;
        QVariantMap map = bb::PpsObject::decode(datagram, &ok);

        if (ok && map.contains(reasonKey)) {
            QString reason = map[reasonKey].toString();
            QVariant data = map[dataKey];

            emit receivedData(reason, data);
        }
        else
            emit receivedData(QString(datagram));
    }
}

void HeadlessCommunication::sendMessage(QString message)
{
    this->sendMessage(message, QVariantMap());
}

void HeadlessCommunication::sendMessage(const QString& reason, const QString& message) {
    this->sendMessage(reason, QVariant(message));
}

void HeadlessCommunication::sendMessage(const QString& reason, const QVariant& variant) {
    QVariantMap map;
    map.insert(reasonKey, reason);
    map.insert(dataKey, variant);

    QByteArray message = bb::PpsObject::encode(map);
    socket->writeDatagram(message, QHostAddress::LocalHost, sendingPort);
}
