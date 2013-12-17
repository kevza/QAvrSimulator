#ifndef LEDMATITEM_H
#define LEDMATITEM_H
#include <QGraphicsWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QTimer>
#include <core/avr_core_builder.h>
#include <Workbench/Tools/ToolItem.h>

class LedMatItem:public QGraphicsWidget, public ToolItemInterface
{
        Q_OBJECT
        public:
            LedMatItem();
            virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
            //toolitem virtual functions
            virtual void attachCore(Avr_Core *currentCore);
            virtual void setSettingsString(QString settings);
            virtual QString getSettingsString();

            QRectF boundingRect() const;

        private slots:
            void toggleChecked();

        protected:
            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
            bool m_update;
        private:
            Avr_Hardware_Interface *mat;
            QStringList leds;
            bool posFixed;
};

#endif // LEDMATITEM_H
