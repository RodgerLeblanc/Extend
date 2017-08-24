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

#include <src/common.hpp>
#include <src/Logger/Logger.h>
#include <src/Settings/Settings.h>

#include <bb/Application>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>
#include <bb/system/SystemUiButton>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        folderCleaner(NULL),
        headlessCommunication(NULL),
        imageFileSignatureChecker(NULL),
        invokeManager(NULL),
        mediaWatcher(NULL),
        notification(NULL),
        settings(NULL),
        fullyLoaded(false)
{
    headlessCommunication = new HeadlessCommunication(Environment::Headless, this);

    Logger::init(headlessCommunication, this);

    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    settings = Settings::instance(headlessCommunication, this);

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Service::init() {
    LOG("Headless started on thread", (int)QThread::currentThreadId());

    imageFileSignatureChecker = new ImageFileSignatureChecker(this);
    invokeManager = new InvokeManager(this);
    mediaWatcher = new MediaWatcher(this);
    notification = new Notification(this);

    connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
    connect(headlessCommunication, SIGNAL(receivedData(QString, QVariant)), this, SLOT(onReceivedData(QString, QVariant)));
    connect(mediaWatcher, SIGNAL(imageWithoutExtensionFound(const QString&)), this, SLOT(onImageWithoutExtensionFound(const QString&)));
    connect(imageFileSignatureChecker,
            SIGNAL(error(QString, ImageFileSignatureCheckerError, QString)),
            this,
            SLOT(onImageFileSignatureCheckerError(QString, ImageFileSignatureCheckerError, QString)));

    NotificationDefaultApplicationSettings notificationDefaultApplicationSettings;
    notificationDefaultApplicationSettings.setPreview(NotificationPriorityPolicy::Allow);
    notificationDefaultApplicationSettings.apply();

    if (SHOULD_ALWAYS_KEEP_HEADLESS_ALIVE) {
        // Next code will reopen this app in case it gets terminated
        QString registerTimerName = APP_NAME.remove(" ");
        InvokeReply* reply = invokeManager->deregisterTimer(registerTimerName);
        reply->deleteLater();

        InvokeRecurrenceRule recurrenceRule(bb::system::InvokeRecurrenceRuleFrequency::Minutely);
        recurrenceRule.setInterval(6); // Minimum valid interval for Minutely frequency
        if (recurrenceRule.isValid()) {
            InvokeTimerRequest invokeTimerRequest(registerTimerName, recurrenceRule, HEADLESS_INVOCATION_TARGET);
            InvokeReply* reply = invokeManager->registerTimer(invokeTimerRequest);
            connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
        }
    }

    if (!settings->contains(SETTINGS_INSTALLED_DATE)) {
        settings->setValue(SETTINGS_INSTALLED_DATE, QDateTime::currentDateTime());
    }

    folderCleaner = new FolderCleaner();
    connect(folderCleaner, SIGNAL(imageWithoutExtensionFound(const QString&)), this, SLOT(onImageWithoutExtensionFound(const QString&)));
    connect(folderCleaner, SIGNAL(initDone()), this, SLOT(onFolderCleanerInitDone()));
    folderCleaner->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), folderCleaner, SLOT(deleteLater()));
    workerThread.start();

    QMetaObject::invokeMethod(folderCleaner, "init", Qt::QueuedConnection);
}

Service::~Service() {
    LOG("~Service()");
    Logger::save();

    workerThread.quit();
    workerThread.wait();
}

void Service::onDeviceActiveChanged(const bool& deviceActive) {
    Q_UNUSED(deviceActive);
}

void Service::onFolderCleanerInitDone() {
    fullyLoaded = true;

    if (!folderCleanerHubMessage.isEmpty()) {
        QString title = QCoreApplication::applicationName();
        QString body = folderCleanerHubMessage;
        QString iconPath = QString::fromUtf8("file://%1/app/native/ExtendLogo_144.png").arg(QDir::currentPath());
        this->notify(title, body, iconPath);

        folderCleanerHubMessage = "";
    }
}

void Service::onInvoked(const bb::system::InvokeRequest & request) {
    QString action = request.action().split(".").last();

    if (action == "TIMER_FIRED") {
        return;
    }

    if (request.action() == HEADLESS_INVOCATION_SHUTDOWN_ACTION) {
        bb::Application::instance()->quit();
    }
    else if (request.action() == HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION) {
        Logger::save();
        headlessCommunication->sendMessage(LOG_READY_FOR_BUG_REPORT);
    }
    else if (request.action() == HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION) {
        QString title = bb::Application::applicationName();
        QString body = STRING(Logger::getLog());
        this->notify(title, body);
    }
}

void Service::onReceivedData(QString reason, QVariant data) {
    Q_UNUSED(reason);
    Q_UNUSED(data);
}

void Service::onImageWithoutExtensionFound(const QString& filePath) {
    QString newFilePath = this->setExtensionToFilePath(filePath);

    if (newFilePath == filePath) {
        return;
    }

    LOG("Service::onImageWithoutExtensionFound():", filePath);
    LOG_VAR(newFilePath);

    if (fullyLoaded) {
        QString title = QCoreApplication::applicationName();
        QString body = QString(tr("Renamed %1")).arg(newFilePath.split("/").last()) + "     ";
        QString iconUrl = newFilePath;
        this->notify(title, body, iconUrl);
    }
    else {
        QString body = QString(tr("Renamed %1")).arg(newFilePath.split("/").last()) + "\n";
        folderCleanerHubMessage += body;
    }
}

QString Service::setExtensionToFilePath(QString filePath) {
    QString newFilePath = imageFileSignatureChecker->setImageExtension(filePath);
    return newFilePath;
}

void Service::onImageFileSignatureCheckerError(QString filePath, ImageFileSignatureCheckerError error, QString errorMessage) {
    if (fullyLoaded)
        LOG("Service::onImageFileSignatureCheckerError():", filePath, STRING(error), errorMessage);

    Q_UNUSED(filePath);
    Q_UNUSED(error);
    Q_UNUSED(errorMessage);
}

void Service::notify(QString title, QString body, QString iconUrl) {
    Notification* notif = this->setNotification(new Notification(this), title, body, iconUrl);
    notif->notify();
}

Notification* Service::setNotification(Notification* notif, QString title, QString body, QString iconUrl) {
    QString message = this->getImageRenamedCountMessage(iconUrl);

    notif->setTitle(title);
    notif->setBody(body + "\n\n" + message);

    if (ImageFileSignatureChecker::isAnImage(iconUrl))
        notif->setIconUrl(QUrl(iconUrl));
    else
        notif->setIconUrl(QUrl(QString::fromUtf8("file://%1/app/native/assets/images/ExtendLogo_1440.png").arg(QDir::currentPath())));

    bb::system::InvokeRequest request;
    request.setUri(QString("bbfiles://%1").arg(iconUrl));
    notif->setInvokeRequest(request);

    return notif;
}

QString Service::getImageRenamedCountMessage(QString iconUrl) {
    int count = this->getImageRenamedCount();

    QString message = QString(tr("%1 successfully renamed %2 files until now. Great, isn't it?"))
                                .arg(QCoreApplication::applicationName())
                                .arg(count);
    message += "\n\n";
    message += QString(tr("Click 'Open' in the bottom of this screen to open your file."));
    message += "\n\n";
    message += QString(tr("The file is located at :\n%1")).arg(this->getCleanFilePath(iconUrl));
    message += "\n\n";
    message += QString(tr("If you want to rate this app in BlackBerry World and tell the world how great it is, click on this link :"));
    message += "\n";
    message += "http://appworld.blackberry.com/webstore/content/60002920";
    return message;
}

int Service::getImageRenamedCount() {
    Settings* settings = Settings::instance();
    int count = settings->value(SETTINGS_IMAGE_RENAMED_COUNT, 0).toInt();
    settings->setValue(SETTINGS_IMAGE_RENAMED_COUNT, ++count);
    return count;
}

QString Service::getCleanFilePath(QString path) {
    path.remove("file://", Qt::CaseInsensitive);
    path.remove("/accounts/1000/shared", Qt::CaseInsensitive);
    path.remove("/accounts/1000/removable", Qt::CaseInsensitive);
    path.remove("/accounts/1000-enterprise/shared", Qt::CaseInsensitive);
    path.remove("/accounts/1000-enterprise/removable", Qt::CaseInsensitive);

    return path;
}
