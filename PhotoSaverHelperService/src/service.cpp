/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "service.hpp"

#include <src/Logger/Logger.h>

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        deviceActive(new DeviceActive(this)),
        folderWatcher(new FolderWatcher(this)),
        headlessCommunication(new HeadlessCommunication(this)),
        invokeManager(new InvokeManager(this))
{
    invokeManager->connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    connect(deviceActive, SIGNAL(deviceActiveChanged(const bool&)), this, SLOT(onDeviceActiveChanged(const bool&)));
    connect(headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));
    connect(folderWatcher, SIGNAL(imageWithoutExtensionFound(const QString&)), this, SLOT(onImageWithoutExtensionFound(const QString&)));

    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();
}

Service::~Service() {
    LOG("~Service()");
    Logger::save();
}

void Service::handleInvoke(const bb::system::InvokeRequest & request) {
    QString action = request.action().split(".").last();

    if (action == "SHUTDOWN") {
        bb::Application::instance()->quit();
    }
}

void Service::onDeviceActiveChanged(const bool& deviceActive) {
    LOG(QString("Service::onDeviceActiveChanged(): " + STRING(deviceActive)));
}

void Service::onImageWithoutExtensionFound(const QString& filePath) {
    LOG(QString("Service::onImageWithoutExtensionFound(): " + filePath));

    ImageFileSignatureChecker* imageFileSignatureChecker = new ImageFileSignatureChecker(filePath, this);
    connect(imageFileSignatureChecker,
            SIGNAL(error(QString, ImageFileSignatureCheckerError, QString)),
            this,
            SLOT(onImageFileSignatureCheckerError(QString, ImageFileSignatureCheckerError, QString)));

    ImageFileExtensionType imageFileType = imageFileSignatureChecker->getImageFileType();
    QString newFilePath = imageFileSignatureChecker->setImageExtension(imageFileType);
    imageFileSignatureChecker->deleteLater();

    LOG_VAR(newFilePath);

    if (newFilePath == filePath) {
        return;
    }

    QString title = bb::Application::applicationName();
    QString body = QString(tr("Renamed %1")).arg(newFilePath.split("/").last());
    QString iconUrl = newFilePath;
    this->notifyTemporarily(title, body, iconUrl);
}

void Service::onImageFileSignatureCheckerError(QString filePath, ImageFileSignatureCheckerError error, QString errorMessage) {
    LOG(QString("Service::onImageFileSignatureCheckerError(): " + filePath + " " + STRING(error) + " " + errorMessage));
}

void Service::onReceivedData(QString data) {
    LOG(QString("Service::onReceivedData(): " + data));

    if (data == "LOG_TO_HUB") {
        QString title = bb::Application::applicationName();
        QString body = STRING(Logger::getLog());
        this->notify(title, body);
    }
}

void Service::notify(QString title, QString body, QString iconUrl) {
    LOG(QString("Service::notify(): " + title + " " + body + " " + iconUrl));

    Notification* notify = this->createNotification(title, body, iconUrl);
    notify->notify();
}

void Service::notifyTemporarily(QString title, QString body, QString iconUrl) {
    LOG(QString("Service::notifyTemporarily(): " + title + " " + body + " " + iconUrl));

    Notification* notify = this->createNotification(title, body, iconUrl);
    notify->setTimestamp(QDateTime::currentDateTime().addYears(-1)); // Hide notification from Hub

    QTimer* notificationKiller = new QTimer(notify);
    connect(notificationKiller, SIGNAL(timeout()), this, SLOT(onNotificationKillerTimeout()));
    notificationKiller->setInterval(3000);
    notificationKiller->setSingleShot(true);
    notificationKiller->start();

    notify->notify();
}

Notification* Service::createNotification(QString title, QString body, QString iconUrl) {
    Notification* notify = new Notification(this);
    notify->setTitle(title);
    notify->setBody(body);
    notify->setIconUrl(QUrl(iconUrl));
    return notify;
}

void Service::onNotificationKillerTimeout() {
    LOG("Service::onNotificationKillerTimeout()");

    QTimer* notificationKiller = qobject_cast<QTimer*>(sender());
    Notification* notify = (Notification*)notificationKiller->parent();

    notify->deleteFromInbox();
    notify->deleteLater();
}
