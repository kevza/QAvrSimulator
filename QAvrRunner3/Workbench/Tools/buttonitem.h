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
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

        void connectHardware(Avr_Hardware_Interface *h);
        void setPin(QString pin);
        void setTexturePressed(QString file);
        void setTextureDepressed(QString file);
        void setPushLow(bool set);
        void bindKey(int key);

    private:
        Avr_Hardware_Interface *hardware;
        QString pin;
        QString pressedTex;
        QString depressedTex;
        bool pushLow;
        int keyId;

};

#endif // BUTTONITEM_H
