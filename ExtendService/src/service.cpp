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
#include <pthread.h>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        headlessCommunication(new HeadlessCommunication(this)),
        invokeManager(new InvokeManager(this)),
        notification(new Notification(this))
{
    LOG("Headless started on thread", pthread_self());

    Notification::deleteAllFromInbox();

    invokeManager->connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    connect(headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));

    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();

    // Next code will reopen Extend in case it gets terminated
    InvokeReply* reply = invokeManager->deregisterTimer(REGISTER_TIMER_NAME);
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));

    InvokeRecurrenceRule recurrenceRule(bb::system::InvokeRecurrenceRuleFrequency::Minutely);
    recurrenceRule.setInterval(6); // Minimum valid interval for Minutely frequency
    if (recurrenceRule.isValid()) {
        InvokeTimerRequest invokeTimerRequest(REGISTER_TIMER_NAME, recurrenceRule, HEADLESS_INVOCATION_TARGET);
        InvokeReply* reply2 = invokeManager->registerTimer(invokeTimerRequest);
        connect(reply2, SIGNAL(finished()), reply2, SLOT(deleteLater()));
    }

    folderWatcher = new FolderWatcher();
    folderWatcher->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), folderWatcher, SLOT(deleteLater()));
    connect(folderWatcher, SIGNAL(imageWithoutExtensionFound(const QString&)), this, SLOT(onImageWithoutExtensionFound(const QString&)));
    workerThread.start();

    QMetaObject::invokeMethod(folderWatcher, "init", Qt::QueuedConnection);
}

Service::~Service() {
    LOG("~Service()");
    Logger::save();

    workerThread.quit();
    workerThread.wait();
}

void Service::onDeviceActiveChanged(const bool& deviceActive) {
    LOG("Service::onDeviceActiveChanged():", STRING(deviceActive));
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

void Service::onReceivedData(QString data) {
    LOG("Service::onReceivedData():", data);
}

void Service::onImageWithoutExtensionFound(const QString& filePath) {
    LOG("Service::onImageWithoutExtensionFound():", filePath);

    QString newFilePath = this->setExtensionToFilePath(filePath);
    LOG_VAR(newFilePath);

    if (newFilePath == filePath) {
        LOG("The file path didn't changed, return.");
        return;
    }

    QString title = QCoreApplication::applicationName();
    QString body = QString(tr("Renamed %1")).arg(newFilePath.split("/").last()) + "     ";
    QString iconUrl = newFilePath;
    this->notify(title, body, iconUrl);
}

QString Service::setExtensionToFilePath(QString filePath) {
    ImageFileSignatureChecker* imageFileSignatureChecker = new ImageFileSignatureChecker(filePath, this);
    connect(imageFileSignatureChecker,
            SIGNAL(error(QString, ImageFileSignatureCheckerError, QString)),
            this,
            SLOT(onImageFileSignatureCheckerError(QString, ImageFileSignatureCheckerError, QString)));

    ImageFileExtensionType imageFileType = imageFileSignatureChecker->getImageFileType();
    QString newFilePath = imageFileSignatureChecker->setImageExtension(imageFileType);
    imageFileSignatureChecker->deleteLater();
    return newFilePath;
}

void Service::onImageFileSignatureCheckerError(QString filePath, ImageFileSignatureCheckerError error, QString errorMessage) {
    LOG("Service::onImageFileSignatureCheckerError():", filePath, STRING(error), errorMessage);
}

void Service::notify(QString title, QString body, QString iconUrl) {
    Notification* notif = this->setNotification(new Notification(this), title, body, iconUrl);
    notif->notify();
    notif->clearEffects();
}

Notification* Service::setNotification(Notification* notif, QString title, QString body, QString iconUrl) {
    notif->setTitle(title);
    notif->setBody(body + "\n\n" + this->getImageRenamedCountMessage());
    notif->setIconUrl(QUrl(iconUrl));
    return notif;
}

QString Service::getImageRenamedCountMessage() {
    int count = this->getImageRenamedCount();

    QString message = QString(tr("%1 successfully renamed %2 images until now. Great, isn't it?"))
                                .arg(QCoreApplication::applicationName())
                                .arg(count);
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
