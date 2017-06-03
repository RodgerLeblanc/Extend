/*
 * DeviceActive.cpp
 *
 *  Created on: 2015-02-10
 *      Author: Roger
 */

#include <src/DeviceActive/DeviceActive.h>
#include <src/Logger/Logger.h>

using namespace bb::device;
using namespace bb::platform;

DeviceActive::DeviceActive(QObject *_parent) :
    QObject(_parent),
    m_deviceInfo(new DeviceInfo(this)),
    m_homeScreen(new HomeScreen(this))
{
    m_screenOn = (m_deviceInfo->activityState() == bb::device::UserActivityState::Active);
    m_isUnlocked = (m_homeScreen->lockState() == bb::platform::DeviceLockState::Unlocked);
    m_bedsideModeActive = m_homeScreen->isBedsideModeActive();

    setDeviceActive(!m_bedsideModeActive && m_isUnlocked && m_screenOn);

    connect(m_homeScreen, SIGNAL(bedsideModeActiveChanged(const bool&)), this, SLOT(onBedsideModeActiveChanged(const bool&)));
    connect(m_homeScreen, SIGNAL(lockStateChanged(bb::platform::DeviceLockState::Type)), this, SLOT(onLockStateChanged(bb::platform::DeviceLockState::Type)));
    connect(m_deviceInfo, SIGNAL(activityStateChanged(bb::device::UserActivityState::Type)), this, SLOT(onActivityStateChanged(bb::device::UserActivityState::Type)));
}

void DeviceActive::onBedsideModeActiveChanged(const bool& newValue) {
    qDebug() << "DeviceActive::onBedsideModeActiveChanged()" << newValue;

    m_bedsideModeActive = newValue;

    if (m_bedsideModeActive) {
        // As user can't interact with a phone on Bedside Mode, we'll consider device as inactive
        setDeviceActive(false);
    }
    else {
        this->onActivityStateChanged(m_deviceInfo->activityState());
    }
}

void DeviceActive::onActivityStateChanged(bb::device::UserActivityState::Type userActivityState) {
    qDebug() << "DeviceActive::onActivityChanged()" << userActivityState;

    m_screenOn = (userActivityState == bb::device::UserActivityState::Active);
    setDeviceActive(!m_bedsideModeActive && m_isUnlocked && m_screenOn);
//    setDeviceActive(m_screenOn);
}

void DeviceActive::onLockStateChanged(bb::platform::DeviceLockState::Type lockState) {
    qDebug() << "DeviceActive::onLockStateChanged()" << lockState;

    m_isUnlocked = (lockState == bb::platform::DeviceLockState::Unlocked);
    setDeviceActive(!m_bedsideModeActive && m_isUnlocked && m_screenOn);
}
