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

#ifndef SERVICE_H_
#define SERVICE_H_

#include <QObject>
#include <QTimer>
#include <src/DeviceActive/DeviceActive.h>
#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/ImageFileSignatureChecker/ImageFileSignatureChecker.h>
#include <src/FolderWatcher/FolderWatcher.h>

namespace bb {
    class Application;
    namespace platform {
        class Notification;
        class NotificationDialog;
    }
    namespace system {
        class InvokeManager;
        class InvokeRequest;
    }
}

class Service: public QObject
{
    Q_OBJECT
public:
    Service();
    virtual ~Service();

private slots:
    void handleInvoke(const bb::system::InvokeRequest &);
    void onDeviceActiveChanged(const bool&);
    void onImageFileSignatureCheckerError(QString, ImageFileSignatureCheckerError, QString);
    void onImageWithoutExtensionFound(const QString&);
    void onNotificationKillerTimeout();
    void onReceivedData(QString);

private:
    bb::platform::Notification* createNotification(QString title, QString body, QString iconUrl);
    void notify(QString title, QString body, QString iconUrl = "");
    void notifyTemporarily(QString title, QString body, QString iconUrl = "");

    DeviceActive* deviceActive;
    FolderWatcher* folderWatcher;
    HeadlessCommunication* headlessCommunication;
    bb::system::InvokeManager* invokeManager;
};

#endif /* SERVICE_H_ */
