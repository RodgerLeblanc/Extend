APP_NAME = ExtendService

CONFIG += qt warn_on

CONFIG(release, debug|release) { 
	DEFINES += QT_NO_DEBUG_OUTPUT
}

include(config.pri)
INCLUDEPATH += ../../$${APP_NAME}/src

LIBS += -lbb -lbbdata -lbbdevice -lbbplatform -lbbsystem
QT += network
 