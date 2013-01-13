#-------------------------------------------------
#
# Project created by QtCreator 2013-01-13T11:36:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAvrRunner3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/release/ -lAvr_Core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/debug/ -lAvr_Core
else:unix: LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/ -lAvr_Core

INCLUDEPATH += $$PWD/../Avr_Core
DEPENDPATH += $$PWD/../Avr_Core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/release/Avr_Core.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/debug/Avr_Core.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/libAvr_Core.a
