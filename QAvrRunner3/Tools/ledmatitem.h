#ifndef LEDMATITEM_H
#define LEDMATITEM_H
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QTimer>
#include <core/avr_core_builder.h>

class LedMatItem: public QGraphicsItem
{
        public:
            LedMatItem(QGraphicsItem *parent = 0);
            void connectHardware(Avr_Hardware_Interface *ledmat);
            QRectF boundingRect() const;
        protected:
            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
            bool m_update;
        private:
            Avr_Hardware_Interface *mat;
            QStringList leds;


};

#endif // LEDMATITEM_H
