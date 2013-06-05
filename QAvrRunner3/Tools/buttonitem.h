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
        void connectHardware(Avr_Hardware_Interface *h);
        void setPin(QString pin);
        void setTexturePressed(QString file);
        void setTextureDepressed(QString file);

    private:
        Avr_Hardware_Interface *hardware;
        QString pin;
        QString pressedTex;
        QString depressedTex;

};

#endif // BUTTONITEM_H
