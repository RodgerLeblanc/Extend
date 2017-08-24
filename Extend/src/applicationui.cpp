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
        headlessCommunication(NULL),
        invokeManager(NULL),
        localeHandler(NULL),
        settings(NULL),
        translator(NULL)
{
    this->invokeHL(HEADLESS_INVOCATION_START_ACTION);

    headlessCommunication = new HeadlessCommunication(Environment::UI, this);

    Logger::init(headlessCommunication, this);

    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    settings = Settings::instance(headlessCommunication, this);

    init();
}

void ApplicationUI::init() {
    invokeManager = new InvokeManager(this);
    localeHandler = new LocaleHandler(this);
    translator = new QTranslator(this);

    connect(headlessCommunication, SIGNAL(receivedData(QString, QVariant)), this, SLOT(onReceivedData(QString, QVariant)));
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

    // Registering QTimer for easy QML access. We add it to bb.cascades to avoid having to import
    // a separate library for such a tiny addition. (Don't worry, it won't slow down your bb.cascades
    // library loading)
    qmlRegisterType<QTimer>("bb.cascades", 1, 0, "QTimer");

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);
}

void ApplicationUI::checkForChangelog() {
    QVariantMap changelogMap = Helpers::safeReadJsonFile(CHANGELOG_FILE).toMap();
    QStringList lastVersionLoadedInSettings = settings->value(SETTINGS_LAST_VERSION_LOADED, "0.0.0.0").toString().split(".");

    QString newChangelogForThisUser;

    QStringList changelogVersions = changelogMap.keys();
    foreach(QString changelog, changelogVersions) {
        newChangelogForThisUser.prepend(changelog + "\n" + changelogMap[changelog].toString() + "\n\n");
    }

    emit newChangelog(newChangelogForThisUser);

    settings->setValue(SETTINGS_LAST_VERSION_LOADED, APP_VERSION);
}

void ApplicationUI::invokeHL(QString action) {
    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(action);
    invokeManager->invoke(request);
}

void ApplicationUI::onReceivedData(QString reason, QVariant data) {
    if (reason == LOG_READY_FOR_BUG_REPORT) {
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
