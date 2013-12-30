#include "ledmatitem.h"
#include <QDebug>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

LedMatItem::LedMatItem()
{
    this->hardware = NULL;
    this->posFixed = true;
    leds  << "C4R0"<< "C3R0"<< "C2R0"<< "C1R0"<< "C0R0"
          << "C4R1"<< "C3R1"<< "C2R1"<< "C1R1"<< "C0R1"
          << "C4R2"<< "C3R2"<< "C2R2"<< "C1R2"<< "C0R2"
          << "C4R3"<< "C3R3"<< "C2R3"<< "C1R3"<< "C0R3"
          << "C4R4"<< "C3R4"<< "C2R4"<< "C1R4"<< "C0R4"
          << "C4R5"<< "C3R5"<< "C2R5"<< "C1R5"<< "C0R5"
          << "C4R6"<< "C3R6"<< "C2R6"<< "C1R6"<< "C0R6";
}

QRectF LedMatItem::boundingRect() const{
   return QRectF(0,0,100,100);
}

void LedMatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
//Create the led mat
    QMap<QString,uint8_t> theMap;
    if (this->hardware){
        theMap = hardware->getOutputs();
    }
    //Draw the leds and paint them the correct color
    painter->setBrush(Qt::gray);
    painter->drawRect(QRect(8,8,62,86));
    for (int x = 0 ; x < 5; x++){
        for (int y = 0; y < 7;y++){
            if (this->hardware){
                if (theMap[leds.at((6 - y) * 5 + (4 - x))] > 0)
                    painter->setBrush(QBrush(QColor(0,theMap[leds.at((6 - y) * 5 + ( 4 - x))],theMap[leds.at((6 - y) * 5 + ( 4 - x))] / 2)));
                else
                    painter->setBrush(Qt::gray);
            }else{
                painter->setBrush(Qt::gray);
            }
            painter->drawEllipse(QRect(10 + (12 * x),10 + (12 * y),10,10));

        }
    }
    if (this->hardware)
        hardware->reset();
}


void LedMatItem::attachCore(Avr_Core *currentCore){
    if (currentCore != NULL) {
        foreach(Avr_Hardware_Interface *h, currentCore->hardware)    {
            if (h->getPluginName() == "AVRLEDMAT"){
                this->hardware = h;
            }
        }
    }else{
        this->hardware = NULL;
    }
}

QString LedMatItem::getSettingsString(){

    return "";
}

void LedMatItem::setSettingsString(QString settings){


}
