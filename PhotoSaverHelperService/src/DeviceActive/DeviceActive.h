/*
 * DeviceActive.h
 *
 *  Created on: 2015-02-10
 *      Author: Roger
 */

#ifndef DEVICEACTIVE_H_
#define DEVICEACTIVE_H_

#include <QObject>

#include <bb/device/DeviceInfo>
#include <bb/platform/DeviceLockState>
#include <bb/platform/HomeScreen>

class DeviceActive : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool deviceActive READ getDeviceActive WRITE setDeviceActive NOTIFY deviceActiveChanged);

public:
    DeviceActive(QObject *_parent = 0);
    virtual ~DeviceActive() {}

    bool getDeviceActive() { return m_deviceActive; }

private slots:
    void onBedsideModeActiveChanged(const bool& newValue);
    void onActivityStateChanged(bb::device::UserActivityState::Type userActivityState);
    void onLockStateChanged(bb::platform::DeviceLockState::Type);

private:
    void setDeviceActive(const bool & newValue) {
        if (m_deviceActive != newValue) {
            m_deviceActive = newValue;
            emit deviceActiveChanged(m_deviceActive);
        }
    }

    bb::device::DeviceInfo* m_deviceInfo;
    bb::platform::HomeScreen* m_homeScreen;

    bool m_bedsideModeActive;
    bool m_deviceActive;
    bool m_isUnlocked;
    bool m_screenOn;

signals:
    void deviceActiveChanged(const bool&);
};

#endif /* DEVICEACTIVE_H_ */
