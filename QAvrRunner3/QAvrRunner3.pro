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
        mainwindow.cpp \
    Tools/ledmatitem.cpp \
    Tools/leditem.cpp \
    Tools/buttonitem.cpp \
    Debugger/debugview.cpp \
    Scene/layoutmanager.cpp \
    Scene/connectiondialog.cpp

HEADERS  += mainwindow.h \
    Tools/ledmatitem.h \
    Tools/leditem.h \
    Tools/buttonitem.h \
    Debugger/debugview.h \
    Scene/layoutmanager.h \
    Scene/connectiondialog.h \
    Tools/toolinterface.h

FORMS    += mainwindow.ui \
    Debugger/debugview.ui \
    Scene/connectiondialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/release/ -lAvr_Core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/debug/ -lAvr_Core
else:unix: LIBS += -L$$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/ -lAvr_Core

INCLUDEPATH += $$PWD/../Avr_Core
DEPENDPATH += $$PWD/../Avr_Core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/release/Avr_Core.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/debug/Avr_Core.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../QAvrSimulator-Builds/Avr_Core-build-Desktop-Release/libAvr_Core.a

RESOURCES += \
    RunnerResources.qrc
