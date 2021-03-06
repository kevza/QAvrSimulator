#include "buttonitem.h"
#include <QFile>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
/**
 * @brief ButtonItem::ButtonItem
 */
ButtonItem::ButtonItem()
{
    pressedTex = ":/icons/Icons/ButtonOn.png";
    depressedTex = ":/icons/Icons/ButtonOff.png";

    this->setPixmap(QPixmap(depressedTex));
    this->setVisible(true);
    this->hardware = NULL;
    //Set default to main press button for lack of a better option
    this->pin = "PIND7";
    pushLow = false;
    keyId = -1;
}

/**
  *@brief ButtonItem::keyPressEvent
  *@param event
  */
void ButtonItem::keyPressEvent(QKeyEvent *event){
    if (event != NULL && event->key() == keyId){
        if (hardware)
            *hardware->getInputs()[this->pin] = pushLow ? 0 : 1;
        this->setPixmap(QPixmap(pressedTex));
        this->update();
    }
    QGraphicsPixmapItem::keyPressEvent(event);
}

/**
 * @brief ButtonItem::keyReleaseEvent
 * @param event
 */
void ButtonItem::keyReleaseEvent(QKeyEvent *event){
    if(event != NULL && event->key() == keyId){
        if (hardware)
            *hardware->getInputs()[this->pin] = pushLow ? 1:0;
        this->setPixmap(QPixmap(depressedTex));
        this->update();
     }
    QGraphicsPixmapItem::keyPressEvent(event);
}


void ButtonItem::bindKey(int key){
    this->keyId = key;
}

/**
 * @brief ButtonItem::mousePressEvent
 * @param event
 */
void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(hardware){
        *hardware->getInputs()[this->pin] = pushLow ? 0 : 1;
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
        *hardware->getInputs()[this->pin] = pushLow ? 1:0;
    } 
    this->setPixmap(QPixmap(depressedTex));
    this->update();
}

void ButtonItem::attachCore(Avr_Core *currentCore){
    if (currentCore != NULL) {
        foreach (Avr_Hardware_Interface *h,currentCore->hardware){
            if (h->getPluginName()=="AVRIO"){
                this->hardware = h;
                *hardware->getInputs()[this->pin] = pushLow ? 1:0;
            }
        }
    }else{
        this->hardware = NULL;
    }
}

QString ButtonItem::getSettingsString(){
    return "";
}

void ButtonItem::setSettingsString(QString settings){


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

void ButtonItem::setPushLow(bool set){
    this->pushLow = set;
}
