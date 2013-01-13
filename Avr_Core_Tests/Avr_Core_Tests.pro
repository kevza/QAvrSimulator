#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T12:32:00
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Avr_Core_Tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    opcodes_tests3.cpp \
    opcode_tests.cpp \
    opcode_tests2.cpp \
    speed_tests.cpp \
    tests.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../AvrSimulator/AvrSimTests/gtest/release/ -lgtest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../AvrSimulator/AvrSimTests/gtest/debug/ -lgtest
else:unix: LIBS += -L$$PWD/../../AvrSimulator/AvrSimTests/gtest/ -lgtest

INCLUDEPATH += $$PWD/../../AvrSimulator/AvrSimTests/gtest
INCLUDEPATH += $$PWD/../../AvrSimulator/AvrSimTests/gtest/include
DEPENDPATH += $$PWD/../../AvrSimulator/AvrSimTests/gtest

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../AvrSimulator/AvrSimTests/gtest/release/gtest.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../AvrSimulator/AvrSimTests/gtest/debug/gtest.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../AvrSimulator/AvrSimTests/gtest/libgtest.a



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/release/ -lAvr_Core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/debug/ -lAvr_Core
else:unix: LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/ -lAvr_Core

INCLUDEPATH += $$PWD/../Avr_Core/
DEPENDPATH += $$PWD/../Avr_Core/core
