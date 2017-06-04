APP_NAME = PhotoSaverHelperService

CONFIG += qt warn_on

CONFIG(release, debug|release) { 
	DEFINES += QT_NO_DEBUG_OUTPUT
}

include(config.pri)

LIBS += -lbb -lbbdata -lbbdevice -lbbplatform -lbbsystem
QT += network
 