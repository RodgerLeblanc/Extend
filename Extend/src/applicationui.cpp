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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>

using namespace bb::cascades;
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
    qDebug() << "ApplicationUI::onReceivedData():" << data;
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

void ApplicationUI::sendToHl(QString message) {
    headlessCommunication->sendMessage(message);
}

void ApplicationUI::shutdown()
{
    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(HEADLESS_INVOCATION_SHUTDOWN_ACTION);
    invokeManager->invoke(request);

    Application::instance()->quit();
}
