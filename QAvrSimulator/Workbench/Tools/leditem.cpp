#include "leditem.h"
#include <QPixmap>

LedItem::LedItem()
{
    offPath = ":/icons/Icons/RedLedOff.png";
    onPath = ":/icons/Icons/BlueLedOn.png";
    pin = "PORTC2";
    this->setPixmap(QPixmap(offPath));
    this->setVisible(true);
    this->hardware = NULL;
}


void LedItem::attachCore(Avr_Core *currentCore){
    if (currentCore != NULL) {
        foreach (Avr_Hardware_Interface *h,currentCore->hardware){
            if (h->getPluginName()=="AVRIO"){
                this->hardware = h;
            }
        }
    }else{
        this->hardware = NULL;
    }

}


void LedItem::setSettingsString(QString settings){



}


QString LedItem::getSettingsString(){

    return QString();
}

void LedItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QMap <QString,unsigned char> pins;
    if (this->hardware){
        pins = this->hardware->getOutputs();
        if (pins.contains(pin)){
            if (pins[pin] >0) {
                this->setPixmap(QPixmap(onPath));
            }else{
                this->setPixmap(QPixmap(offPath));
            }
        }
    }
    QGraphicsPixmapItem::paint(painter,option,widget);
}
