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

#include "common.hpp"
#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Logger/Logger.h>
#include <src/Settings/Settings.h>

#include <QObject>
#include <QTimer>
#include <bb/platform/Notification>

#include <src/ImageFileSignatureChecker/ImageFileSignatureChecker.h>
#include <src/FolderCleaner/FolderCleaner.h>
#include <src/MediaWatcher/MediaWatcher.h>

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
    void init();
    void onInvoked(const bb::system::InvokeRequest &);
    void onDeviceActiveChanged(const bool&);
    void onFolderCleanerInitDone();
    void onImageFileSignatureCheckerError(QString, ImageFileSignatureCheckerError, QString);
    void onImageWithoutExtensionFound(const QString&);
    void onReceivedData(QString, QVariant);

private:
    QString getCleanFilePath(QString path);
    int getImageRenamedCount();
    QString getImageRenamedCountMessage(QString iconUrl);
    void notify(QString title, QString body, QString iconUrl = "");
    void notifyTemporarily(QString title, QString body, QString iconUrl = "");
    QString setExtensionToFilePath(QString filePath);
    bb::platform::Notification* setNotification(bb::platform::Notification* notif, QString title, QString body, QString iconUrl);

    FolderCleaner* folderCleaner;
    HeadlessCommunication* headlessCommunication;
    ImageFileSignatureChecker* imageFileSignatureChecker;
    bb::system::InvokeManager* invokeManager;
    MediaWatcher* mediaWatcher;
    bb::platform::Notification* notification;
    Settings* settings;

    QString folderCleanerHubMessage;
    bool fullyLoaded;
    QThread workerThread;
};

#endif /* SERVICE_H_ */
