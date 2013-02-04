#ifndef BUTTONITEM_H
#define BUTTONITEM_H
#include <core/core.h>
#include <QGraphicsPixmapItem>

class ButtonItem : public QGraphicsPixmapItem
{
    public:
        ButtonItem();
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        void setHardware(Avr_Hardware_Interface *h);
    private:
        Avr_Hardware_Interface *hardware;

};

#endif // BUTTONITEM_H
