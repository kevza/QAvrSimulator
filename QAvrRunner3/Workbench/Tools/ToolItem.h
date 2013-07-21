#ifndef TOOLITEM_H
#define TOOLITEM_H
#include <core/core.h>
#include <QString>

class ToolItemInterface{
    public:
    virtual void attachCore(Avr_Core *currentCore) = 0;
    virtual QString getSettingsString() = 0;
    virtual void setSettingsString(QString settings) = 0;








};

#endif // TOOLITEM_H
