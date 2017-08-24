APP_NAME = Extend
HEADLESS_SERVICE = $${APP_NAME}Service

CONFIG += qt warn_on cascades10

CONFIG(release, debug|release) { 
	DEFINES += QT_NO_DEBUG_OUTPUT
}

include(config.pri)
INCLUDEPATH += ../../$${HEADLESS_SERVICE}/src

LIBS += -lbb -lbbdata -lbbdevice -lbbplatform -lbbsystem
