#ifndef LEDMATITEM_H
#define LEDMATITEM_H
#include <QGraphicsWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QTimer>
#include <core/avr_core_builder.h>
#include <Workbench/Tools/ToolItem.h>

class LedMatItem: public ToolItem {
        Q_OBJECT
        public:
            LedMatItem();

            //toolitem virtual functions
            virtual void attachCore(Avr_Core *currentCore);
            virtual void setSettingsString(QString settings);
            virtual QString getSettingsString();

            QRectF boundingRect() const;

        protected:
            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
            bool m_update;

        private:
            QStringList leds;
            bool posFixed;
};

#endif // LEDMATITEM_H
