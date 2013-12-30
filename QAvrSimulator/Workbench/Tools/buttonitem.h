#ifndef BUTTONITEM_H
#define BUTTONITEM_H
#include <core/core.h>
#include <Workbench/Tools/ToolItem.h>

#include <QPainter>

enum {PRESSED, RELEASED};

class ButtonItem : public ToolItem
{
    public:
        ButtonItem();
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        QRectF boundingRect() const;

        //toolitem virtual functions
        virtual void attachCore(Avr_Core *currentCore);
        virtual void setSettingsString(QString settings);
        virtual QString getSettingsString();

        //Button Functions
        void setPin(QString pin);
        void setTexturePressed(QString file);
        void setTextureDepressed(QString file);
        void setPushLow(bool set);
        void bindKey(int key);
    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        QString pin;
        QString pressedTex;
        QString depressedTex;
        bool pushLow;
        int buttonState;
        int keyId;

};

#endif // BUTTONITEM_H
