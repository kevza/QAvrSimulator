#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T12:18:17
#
#-------------------------------------------------

QT      -= gui
QT      += core

TARGET = Avr_Core
TEMPLATE = lib
CONFIG += staticlib

DEFINES += AVR_CORE_LIBRARY

SOURCES += \
    core/avr_memory.cpp \
    core/avr_registers.cpp \
    core/core.cpp \
    core/flash.cpp \
    core/avr_core_builder.cpp

HEADERS +=\
    Avr_Core_global.h \
    core/avr_memory.h \
    core/avr_registers.h \
    core/core.h \
    core/flash.h \
    core/opcode_masks.h \
    core/avr_core_builder.h \
    interface/avr_hardware_interface.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
