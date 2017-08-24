/*
 * common.hpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_


/*** ▼ ▼ ▼ CHANGE VALUES BETWEEN COMMENTS FOR EVERY NEW APP ▼ ▼ ▼ ***/
#define PACKAGE_NAME                                QString("com.CellNinja.Extend")
#define MOMENTICS_PROJECT_NAME                      QString("Extend")
#define BUG_REPORT_EMAIL_TO                         QString("support@CellNinja.ca")
#define SHOULD_ALWAYS_KEEP_HEADLESS_ALIVE           true
#define HEADLESS_COMMUNICATION_PORT_UI              17265
#define HEADLESS_COMMUNICATION_PORT_HL              HEADLESS_COMMUNICATION_PORT_UI + 1
/*** ▲ ▲ ▲ CHANGE VALUES BETWEEN COMMENTS FOR EVERY NEW APP ▲ ▲ ▲ ***/





// App description
#define APP_NAME                                    QCoreApplication::applicationName()
#define APP_VERSION                                 QCoreApplication::applicationVersion()

// Invocation targets and actions
#define UI_INVOCATION_TARGET                        PACKAGE_NAME
#define HEADLESS_INVOCATION_TARGET                  PACKAGE_NAME + QString("Service")
#define HEADLESS_INVOCATION_START_ACTION            HEADLESS_INVOCATION_TARGET + QString(".START")
#define HEADLESS_INVOCATION_SHUTDOWN_ACTION         HEADLESS_INVOCATION_TARGET + QString(".SHUTDOWN")
#define HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION  HEADLESS_INVOCATION_TARGET + QString(".SEND_BUG_REPORT")
#define HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION  HEADLESS_INVOCATION_TARGET + QString(".SEND_LOG_TO_HUB")

// Path to files
#define LOG_FILE                                    "data/log.txt"
#define SETTINGS_FILE                               "data/settings.json"
#define LOG_SHAREWITH_PATH                          "sharewith/pim/log.txt"
#define CHANGELOG_FILE                              "app/native/assets/Changelog/changelog.json"

// Read and write default retry values
#define READ_FILE_MAX_RETRY                         30
#define WRITE_FILE_MAX_RETRY                        30
#define READ_FILE_ERROR_SLEEP_TIME_MS               1000 * 100
#define WRITE_FILE_ERROR_SLEEP_TIME_MS              1000 * 100

// Those are the keys for the map passed in HeadlessCommunication protocol
#define HEADLESS_COMMUNICATION_REASON               QString("HEADLESS_COMMUNICATION_REASON")
#define HEADLESS_COMMUNICATION_DATA                 QString("HEADLESS_COMMUNICATION_DATA")

// Reasons used with HeadlessCommunication
#define LOG_READY_FOR_BUG_REPORT                    QString("LOG_READY_FOR_BUG_REPORT")
#define LOGGER_LOG_THIS                             QString("LOGGER_LOG_THIS")
#define SETTINGS_CLEAR                              QString("SETTINGS_CLEAR")
#define SETTINGS_REMOVE_KEY                         QString("SETTINGS_REMOVE_KEY")
#define SETTINGS_SAVE_THIS                          QString("SETTINGS_SAVE_THIS")
#define SETTINGS_FILE_UPDATED                       QString("SETTINGS_FILE_UPDATED")
#define SETTINGS_INSTALLED_DATE                     QString("SETTINGS_INSTALLED_DATE")
#define SETTINGS_LAST_VERSION_LOADED                QString("SETTINGS_LAST_VERSION_LOADED")
#define SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY      QString("SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY")
#define SETTINGS_IMAGE_RENAMED_COUNT                QString("IMAGE_RENAMED_COUNT")

// Default toString() format for DateTime and Time
#define DATETIME_TO_STRING                          "yyyy/MM/dd hh:mm:ss"
#define TIME_TO_STRING                              "hh:mm:ss"

// Used to differentiate in what environment the code is running
namespace Environment {
    enum Type { Unknown, UI, Headless };
}

#endif /* COMMON_HPP_ */
