#include "buttonitem.h"
#include <QFile>
#include <QDebug>
#include <QMutex>
/**
 * @brief ButtonItem::ButtonItem
 */
ButtonItem::ButtonItem()
{
    this->setPixmap(QPixmap(":/icons/Icons/Stop.png"));
    this->setVisible(true);
    this->hardware = NULL;
    //Set default to main press button for lack of a better option
    this->pin = "PIND7";
    pressedTex = ":/icons/Icons/Play.png";
    depressedTex = ":/icons/Icons/Stop.png";
}


/**
 * @brief ButtonItem::mousePressEvent
 * @param event
 */
void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(hardware){
        *hardware->getInputs()[this->pin] = 1;
    }

    this->setPixmap(QPixmap(pressedTex));
    this->update();
}

/**
 * @brief ButtonItem::mouseReleaseEvent
 * @param event
 */
void ButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(hardware){
        *hardware->getInputs()[this->pin] = 0;
    } 
    this->setPixmap(QPixmap(depressedTex));
    this->update();
}


void ButtonItem::connectHardware(Avr_Hardware_Interface *core){
    this->hardware = core;
}

void ButtonItem::setPin(QString pin){
    this->pin = pin;
}

void ButtonItem::setTexturePressed(QString file){
    pressedTex = file;
}

void ButtonItem::setTextureDepressed(QString file){
    depressedTex = file;
}
