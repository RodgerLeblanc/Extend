APP_NAME = PhotoSaverHelper
HEADLESS_SERVICE = $${APP_NAME}Service

CONFIG += qt warn_on cascades10

include(config.pri)
INCLUDEPATH += ../../$${HEADLESS_SERVICE}/src

LIBS += -lbb -lbbdata -lbbsystem
