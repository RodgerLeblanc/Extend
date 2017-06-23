/*
 * common.hpp
 *
 *  Created on: May 30, 2017
 *      Author: roger
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#define REQUEST_KEY                                 "REQUEST_KEY"
#define REQUEST_KEY_KEY                             "REQUEST_KEY_KEY"
#define REQUEST_VALUE_KEY                           "REQUEST_VALUE_KEY"
#define REQUEST_DEFAULT_VALUE_KEY                   "REQUEST_DEFAULT_VALUE_KEY"
#define REQUEST_ALL_SETTINGS_KEY                    "REQUEST_ALL_SETTINGS_KEY"

#define REQUEST_GET_ALL_SETTINGS                    "REQUEST_GET_ALL_SETTINGS"
#define REQUEST_GET_SETTINGS                        "REQUEST_GET_SETTINGS"
#define REQUEST_SET_SETTINGS                        "REQUEST_SET_SETTINGS"

#define LOG_FILE "data/log.txt"

#define SETTINGS_UI_LISTENING_PORT                  15837
#define SETTINGS_HEADLESS_LISTENING_PORT            15838

#define SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY      "SETTINGS_FILESYSTEMWATCHER_FOLDERS_KEY"
#define SETTINGS_IMAGE_RENAMED_COUNT                "IMAGE_RENAMED_COUNT"

#define REGISTER_TIMER_NAME                         "RESTART_EXTEND"

#define UI_INVOCATION_TARGET                        "com.CellNinja.Extend"
#define HEADLESS_INVOCATION_TARGET                  "com.CellNinja.ExtendService"
#define HEADLESS_INVOCATION_START_ACTION            "com.CellNinja.ExtendService.START"
#define HEADLESS_INVOCATION_SHUTDOWN_ACTION         "com.CellNinja.ExtendService.SHUTDOWN"
#define HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION  "com.CellNinja.ExtendService.SEND_BUG_REPORT"
#define HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION  "com.CellNinja.ExtendService.SEND_LOG_TO_HUB"

#define LOG_READY_FOR_BUG_REPORT                    "LOG_READY_FOR_BUG_REPORT"

#define UI_LISTENING_PORT                           17265
#define HEADLESS_LISTENING_PORT                     17266

#endif /* COMMON_HPP_ */
