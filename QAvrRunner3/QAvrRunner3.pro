#-------------------------------------------------
#
# Project created by QtCreator 2013-01-13T11:36:02
#
#-------------------------------------------------
CONFIG += serialport
QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAvrRunner3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        Debugger/debugview.cpp \
    Workbench/layoutmanager.cpp \
    Workbench/Tools/buttonitem.cpp \
    Workbench/Tools/leditem.cpp \
    Workbench/Tools/ledmatitem.cpp

HEADERS  += mainwindow.h \
                Debugger/debugview.h \
    Workbench/layoutmanager.h \
    Workbench/Tools/buttonitem.h \
    Workbench/Tools/leditem.h \
    Workbench/Tools/ledmatitem.h \
    Workbench/Tools/ToolItem.h

FORMS    += mainwindow.ui \
        Debugger/debugview.ui


RESOURCES += \
    RunnerResources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Avr_Core/release/ -lAvr_Core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Avr_Core/debug/ -lAvr_Core
else:unix: LIBS += -L$$OUT_PWD/../Avr_Core/ -lAvr_Core

INCLUDEPATH += $$PWD/../Avr_Core
DEPENDPATH += $$PWD/../Avr_Core

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Avr_Core/release/Avr_Core.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Avr_Core/debug/Avr_Core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Avr_Core/libAvr_Core.a
