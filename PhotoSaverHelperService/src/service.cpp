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
//        notify(new Notification(this)),
        invokeManager(new InvokeManager(this)),
        folderWatcher(new FolderWatcher(this)),
        headlessCommunication(new HeadlessCommunication(this))
{
    invokeManager->connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    connect(headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));
    connect(folderWatcher, SIGNAL(imageWithoutExtensionFound(const QString&)), this, SLOT(onImageWithoutExtensionFound(const QString&)));

    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();

    folderWatcher->addFolder("/accounts/1000/removable/sdcard/photos/");
}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    QString action = request.action().split(".").last();

    if (action == "SHUTDOWN") {
        bb::Application::instance()->quit();
    }
}

void Service::onImageWithoutExtensionFound(const QString& filePath) {
    qDebug() << "onImageWithoutExtensionFound():" << filePath;

    ImageFileSignatureChecker* imageFileSignatureChecker = new ImageFileSignatureChecker(filePath, this);
    ImageFileType::ImageFileType imageFileType = imageFileSignatureChecker->getImageFileType();
    QString newFilePath = imageFileSignatureChecker->setImageExtension(imageFileType);
    imageFileSignatureChecker->deleteLater();

    qDebug() << "newFilePath" << newFilePath;
}

void Service::onReceivedData(QString data) {
    qDebug() << "Service::onReceivedData():" << data;
}
