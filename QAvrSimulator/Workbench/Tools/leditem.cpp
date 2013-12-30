#include "leditem.h"
#include <QPixmap>

LedItem::LedItem()
{
    offPath = QPixmap(":/icons/Icons/RedLedOff.png");
    onPath = QPixmap(":/icons/Icons/BlueLedOn.png");
    pin = "PORTC2";
    //this->setPixmap(QPixmap(offPath));
    this->setVisible(true);
    this->hardware = NULL;
    this->update();
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
    qDebug() << "Painting Led Item";
    QMap <QString,unsigned char> pins;
    if (this->hardware){
        pins = this->hardware->getOutputs();
        if (pins.contains(pin)){
            if (pins[pin] >0) {
                painter->drawPixmap(0, 0, onPath);
            }else{
                painter->drawPixmap(0, 0, offPath);
            }
        }
    }else{
        painter->drawPixmap(0, 0, offPath);
    }
}

QRectF LedItem::boundingRect() const{
     return QRectF(10,10,68,92);
}
