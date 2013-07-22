#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T21:30:41
#
#-------------------------------------------------



TARGET = $$qtLibraryTarget(avr_uart)
TEMPLATE = lib
CONFIG += plugin serialport
QT  += core gui

SOURCES += \
    avr_uart.cpp \
    serial.cpp

HEADERS += \
    avr_uart.h \
    serial.h

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
