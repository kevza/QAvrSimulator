#ifndef TOOLITEM_H
#define TOOLITEM_H
#include <core/core.h>

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QString>
#include <QTimer>

enum {LOCKED, WAITING, UNLOCKED};

class ToolItem : public QGraphicsWidget {
    Q_OBJECT
    public:
        ToolItem() {lockState = LOCKED;}
        virtual void attachCore(Avr_Core *currentCore) = 0;
        virtual QString getSettingsString() = 0;
        virtual void setSettingsString(QString settings) = 0;

        //For Mouse relocations
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        //virtual void mouseMoveEvent(QGraphicsSceneMouseEVent *event);

    protected:
        Avr_Hardware_Interface *hardware;

    private:
        int lockState;

    private slots:
        void timeOut();
};

#endif // TOOLITEM_H
