#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T21:30:41
#
#-------------------------------------------------



TARGET = $$qtLibraryTarget(avr_timer_16b)
TEMPLATE = lib
CONFIG += plugin
QT  += core gui

SOURCES += \
    avr_timer_16b.cpp

HEADERS += \
    avr_timer_16b.h

DESTDIR = ..\QAvrRunner3\plugins

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS +=
