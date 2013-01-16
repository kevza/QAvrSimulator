#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T21:30:41
#
#-------------------------------------------------



TARGET = $$qtLibraryTarget(avr_ledmat)
TEMPLATE = lib
CONFIG += plugin
QT  += core gui

SOURCES += \
    avr_ledmat.cpp

HEADERS += \
    interface/avr_hardware_interface.h \
    avr_ledmat.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS +=
