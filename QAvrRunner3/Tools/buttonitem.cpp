#include "buttonitem.h"
#include <QFile>
#include <QDebug>
#include <QMutex>
/**
 * @brief ButtonItem::ButtonItem
 */
ButtonItem::ButtonItem()
{
    QPixmap *map = new QPixmap();
    map->load(":/icons/Icons/Stop.png");
    if (!QFile::exists(":/icons/Icons/Stop.png")){
        qDebug() << "Bummer dude";

    }
    this->setPixmap(*map);
    this->setFlag(QGraphicsItem::ItemIsMovable,true);
    this->setVisible(true);
    this->hardware = NULL;
}


/**
 * @brief ButtonItem::mousePressEvent
 * @param event
 */
void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(hardware){
        QMutex m;
        m.lock();
        *hardware->getInputs()["PIND"] |= (1 << 7);
        m.unlock();
    }


    //Keep QGraphics Item default behaviours
    QGraphicsItem::mousePressEvent(event);
}


/**
 * @brief ButtonItem::mouseReleaseEvent
 * @param event
 */
void ButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(hardware){
        QMutex m;
        m.lock();
        *hardware->getInputs()["PIND"] &= ~(1 << 7);
        m.unlock();
    }
    //Keep QGraphics Item default behaviours
    QGraphicsItem::mouseReleaseEvent(event);
}


void ButtonItem::setHardware(Avr_Hardware_Interface *core){
    this->hardware = core;
}
