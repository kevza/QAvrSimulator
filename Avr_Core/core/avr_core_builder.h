#ifndef AVR_CORE_BUILDER_H
#define AVR_CORE_BUILDER_H

#include "core.h"
#include "avr_memory.h"
#include "avr_registers.h"
#include "interface/avr_hardware_interface.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QPluginLoader>
#include <QVector>
#include <QDebug>

#include <stdint.h>
#include <string>
#include <sstream>
#include <fstream>

class Avr_Core_Builder : public QObject
{
    Q_OBJECT
    public:
        Avr_Core_Builder(QObject *parent = 0);
        Avr_Core* loadCore(QString mmcu);
        int sizeToInt(string size);
        bool loadProgram(string file);
        uint8_t *getRegPtr(string loc);
        int getRegLoc(string loc);
        Avr_Hardware_Interface *interface;

    private:
        Avr_Core *core;
};

#endif // AVR_CORE_BUILDER_H
