#ifndef LEDITEM_H
#define LEDITEM_H
#include <Workbench/Tools/ToolItem.h>

#include <QPainter>

class LedItem :public ToolItem
{
public:
    LedItem();
    //toolitem virtual functions
    virtual void attachCore(Avr_Core *currentCore);
    virtual void setSettingsString(QString settings);
    virtual QString getSettingsString();
    QRectF boundingRect() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool m_update();
private:
    QPixmap onPath;
    QPixmap offPath;
    QString pin;
};

#endif // LEDITEM_H
