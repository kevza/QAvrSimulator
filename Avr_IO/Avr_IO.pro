#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T21:30:41
#
#-------------------------------------------------



TARGET = $$qtLibraryTarget(avr_io)
TEMPLATE = lib
CONFIG += plugin
QT  += widgets

SOURCES += \
    avr_io.cpp

HEADERS += \
    avr_io.h

DESTDIR = ..\QAvrSimulator\plugins

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS +=
