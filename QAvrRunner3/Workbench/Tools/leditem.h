#ifndef LEDITEM_H
#define LEDITEM_H
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <Workbench/Tools/ToolItem.h>

class LedItem :public ToolItemInterface, public QGraphicsPixmapItem
{
public:
    LedItem();
    //toolitem virtual functions
    virtual void attachCore(Avr_Core *currentCore);
    virtual void setSettingsString(QString settings);
    virtual QString getSettingsString();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool m_update();
private:
    QString onPath;
    QString offPath;
    QString pin;
    Avr_Hardware_Interface *hardware;
};

#endif // LEDITEM_H
