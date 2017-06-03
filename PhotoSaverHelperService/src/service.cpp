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
#include "src/ImageFileSignatureChecker/ImageFileSignatureChecker.h"

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

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    QString action = request.action().split(".").last();

    if (action == "SHUTDOWN") {
        bb::Application::instance()->quit();
    }
}

void Service::onDeviceActiveChanged(const bool& deviceActive) {
    qDebug() << "Service::onDeviceActiveChanged():" << QString(deviceActive ? "true" : "false");
}

void Service::onImageWithoutExtensionFound(const QString& filePath) {
    qDebug() << "Service::onImageWithoutExtensionFound():" << filePath;

    ImageFileSignatureChecker* imageFileSignatureChecker = new ImageFileSignatureChecker(filePath, this);
    ImageFileExtension::Type imageFileType = imageFileSignatureChecker->getImageFileType();
    QString newFilePath = imageFileSignatureChecker->setImageExtension(imageFileType);
    imageFileSignatureChecker->deleteLater();

    qDebug() << "newFilePath" << newFilePath;
    QString title, body, iconUrl;
    if (newFilePath != filePath) {
        title = bb::Application::applicationName();
        body = QString(tr("Renamed %1")).arg(newFilePath.split("/").last());
        iconUrl = newFilePath;
    }
    else {
        title = bb::Application::applicationName();
        body = QString(tr("Error renaming %1")).arg(newFilePath.split("/").last());
    }
    this->notify(title, body, iconUrl);
}

void Service::onReceivedData(QString data) {
    qDebug() << "Service::onReceivedData():" << data;
}

void Service::notify(QString title, QString body, QString iconUrl) {
    qDebug() << "Service::notify():" << title << body << iconUrl;

    bb::platform::Notification* notify = new Notification(this);
    notify->setTitle(title);
    notify->setBody(body);
    notify->setIconUrl(QUrl(iconUrl));
    notify->setTimestamp(QDateTime::currentDateTime().addYears(-1)); // Hide notification from Hub

    QTimer* notificationKiller = new QTimer(notify);
    connect(notificationKiller, SIGNAL(timeout()), this, SLOT(onNotificationKillerTimeout()));
    notificationKiller->setInterval(3000);
    notificationKiller->setSingleShot(true);
    notificationKiller->start();

    notify->notify();
}

void Service::onNotificationKillerTimeout() {
    qDebug() << "Service::onNotificationKillerTimeout()";

    QTimer* notificationKiller = qobject_cast<QTimer*>(sender());
    Notification* notify = (Notification*)notificationKiller->parent();

    notify->deleteFromInbox();
    notify->deleteLater();
}
