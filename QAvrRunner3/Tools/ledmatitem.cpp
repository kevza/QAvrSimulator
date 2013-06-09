#include "ledmatitem.h"
#include <QDebug>

LedMatItem::LedMatItem(QGraphicsItem *parent):QGraphicsItem(parent)
{
    this->mat = NULL;
    leds  << "C4R0"<< "C3R0"<< "C2R0"<< "C1R0"<< "C0R0"
          << "C4R1"<< "C3R1"<< "C2R1"<< "C1R1"<< "C0R1"
          << "C4R2"<< "C3R2"<< "C2R2"<< "C1R2"<< "C0R2"
          << "C4R3"<< "C3R3"<< "C2R3"<< "C1R3"<< "C0R3"
          << "C4R4"<< "C3R4"<< "C2R4"<< "C1R4"<< "C0R4"
          << "C4R5"<< "C3R5"<< "C2R5"<< "C1R5"<< "C0R5"
          << "C4R6"<< "C3R6"<< "C2R6"<< "C1R6"<< "C0R6";
}

QRectF LedMatItem::boundingRect() const{
    return QRectF(10,10,68,92);
}

void LedMatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
//Create the led mat
    QMap<QString,uint8_t> theMap;
    if (this->mat){
        theMap = mat->getOutputs();
    }
    //Draw the leds and paint them the correct color
    for (int x = 0 ; x < 5; x++){
        for (int y = 0; y < 7;y++){
            if (this->mat){
                if (theMap[leds.at(y * 5 + x)] > 0)
                    painter->setBrush(QBrush(QColor(255,0,0)));
                else
                    painter->setBrush(Qt::black);
            }else{
                painter->setBrush(Qt::black);
            }
            painter->drawRect(QRect(10 + (12 * x),10 + (12 * y),10,10));
        }
    }
}

void LedMatItem::connectHardware(Avr_Hardware_Interface *ledmat){
    this->mat = ledmat;
}

