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

#include "applicationui.hpp"
#include "common.hpp"

#include <src/Logger/Logger.h>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>

#include <bb/PpsObject>
#include <QMutex>
#include <QMutexLocker>
#include <bb/data/JsonDataAccess>

#include <bb/PackageInfo>
#include <bb/device/HardwareInfo>
#include <bb/platform/PlatformInfo>

using namespace bb::cascades;
using namespace bb::device;
using namespace bb::platform;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject(),
        translator(new QTranslator(this)),
        localeHandler(new LocaleHandler(this)),
        invokeManager(new InvokeManager(this)),
        headlessCommunication(new HeadlessCommunication(this))
{
    connect(headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));
    connect(localeHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));

    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(HEADLESS_INVOCATION_START_ACTION);
    invokeManager->invoke(request);

    onSystemLanguageChanged();

    deviceInfoMap.insert("height", getenv("HEIGHT"));
    deviceInfoMap.insert("width", getenv("WIDTH"));
    deviceInfoMap.insert("coverWidth", getenv("COVERWIDTH"));
    deviceInfoMap.insert("coverHeight", getenv("COVERHEIGHT"));
    emit this->deviceInfoChanged(deviceInfoMap);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);
}

void ApplicationUI::onReceivedData(QString data) {
    LOG("ApplicationUI::onReceivedData():", data);

    if (data == LOG_READY_FOR_BUG_REPORT) {
        this->sendBugReport();
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Extend_%1").arg(locale_string);
    if (translator->load(file_name, "app/native/qm")) {
    QCoreApplication::instance()->installTranslator(translator);
    }
}

void ApplicationUI::sendBugReport() {
    HardwareInfo hardwareInfo;
    PlatformInfo platformInfo;

    InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.COMPOSE");
    request.setMimeType("message/rfc822");
    QVariantMap data;
    data["to"] = "support@cellninja.ca";
    data["subject"] = "Bug Report for " + bb::Application::applicationName() + " " + bb::Application::applicationVersion() + " (" + hardwareInfo.modelName() + " " + platformInfo.osVersion() + ")";
    data["body"] = "Hi CellNinja, I've found this bug in the app:\n\n\nHere's the steps to reproduce:\n\n";

    QString logPath = QDir::currentPath() + "/sharewith/pim/log.txt";

    QMutex mutex;
    QMutexLocker locker(&mutex);

    bb::data::JsonDataAccess jda;
    QStringList log = jda.load(LOG_FILE).toMap()["log"].toStringList();

    locker.unlock();

    QVariantMap logMap;
    logMap.insert("applicationVersion", bb::Application::instance()->applicationVersion());
    logMap.insert("Time", QDateTime::currentDateTime());
    logMap.insert("log", log);
    jda.save(logMap, logPath);

    data["attachment"] = logPath;
    QVariantMap moreData;
    moreData.insert("data", data);

    bool ok;
    request.setData(bb::PpsObject::encode(moreData, &ok));

    if (!ok) {
        LOG("PpsObject wasn't able to encode data");
    }

    invokeManager->invoke(request);
}

void ApplicationUI::sendToHl(QString message) {
    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(QString(HEADLESS_INVOCATION_TARGET) + "." + message);
    invokeManager->invoke(request);
}

void ApplicationUI::shutdown() {
    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(HEADLESS_INVOCATION_SHUTDOWN_ACTION);
    invokeManager->invoke(request);

    Application::instance()->quit();
}
