/*
 * helpers.hpp
 *
 *  Created on: Jul 15, 2017
 *      Author: roger
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include "common.hpp"

#include <bb/data/JsonDataAccess>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>
#include <QStringList>
#include <unistd.h>

class Helpers : public QObject
{
    Q_OBJECT

public:
    static QString printEnvironment(bool isInUiThread) {
        return QString(isInUiThread ? "UI" : "HL");
    }

    static QVariant safeReadJsonFile(QString path) {
        bb::data::JsonDataAccess jda;
        int maxRetries = READ_FILE_MAX_RETRY;
        QMutex mutex;
        QVariant variant;

        do {
            QMutexLocker locker(&mutex);
            variant = jda.load(path);
            locker.unlock();

            if (jda.hasError()) {
                bb::data::DataAccessError error = jda.error();
                if (error.errorType() == bb::data::DataAccessErrorType::SourceNotFound) {
                    QFile jsonFile(path);
                    if (!jsonFile.exists())
                        jda.save(variant, path);

                    jsonFile.deleteLater();
                }

                usleep(READ_FILE_ERROR_SLEEP_TIME_MS);
            }
        } while (--maxRetries >= 0 && jda.hasError());

        return variant;
    }

    static void safeWriteJsonFile(QVariant variant, QString path) {
        bb::data::JsonDataAccess jda;
        QMutex mutex;
        int maxRetries = WRITE_FILE_MAX_RETRY;

        do {
            QMutexLocker locker(&mutex);
            jda.save(variant, path);
            locker.unlock();

            if (jda.hasError()) {
                usleep(WRITE_FILE_ERROR_SLEEP_TIME_MS);
            }
        } while (--maxRetries >= 0 && jda.hasError());
    }
};

#endif /* HELPERS_HPP_ */
