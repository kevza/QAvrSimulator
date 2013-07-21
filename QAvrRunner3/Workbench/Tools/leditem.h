#ifndef LEDITEM_H
#define LEDITEM_H
#include <QGraphicsItem>
#include <Workbench/Tools/ToolItem.h>

class LedItem :public ToolItemInterface, public QGraphicsItem
{
public:
    LedItem();
    //toolitem virtual functions
    virtual void attachCore(Avr_Core *currentCore);
    virtual void setSettingsString(QString settings);
    virtual QString getSettingsString();

};

#endif // LEDITEM_H
