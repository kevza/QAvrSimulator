#include "ToolItem.h"


//For Mouse relocations
void ToolItem::mousePressEvent(QGraphicsSceneMouseEvent* /* event*/){
    qDebug() << "Mouse Pressed";
    lockState = WAITING;
    QTimer::singleShot(2000, this, SLOT(timeOut()));

}

void ToolItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* /* event*/){
    qDebug() << "Mouse Released";
    if (lockState == UNLOCKED){
        this->setFlag(QGraphicsWidget::ItemIsMovable, false);
    }

    lockState = LOCKED;
}

void ToolItem::timeOut(){
    if (lockState == WAITING) {
        lockState = UNLOCKED;
        this->setFlag(QGraphicsWidget::ItemIsMovable, true);
    }


}
