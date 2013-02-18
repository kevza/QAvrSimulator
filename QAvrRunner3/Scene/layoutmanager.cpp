#include "layoutmanager.h"
#include <QTimer>
/**
 * @brief LayoutManager::LayoutManager
 * @param parent
 */
LayoutManager::LayoutManager(QObject *parent) : QGraphicsScene(parent)
{
    //Keeps the Layout updating at the correct speed
    QTimer* timer = new QTimer();
    timer->setInterval(20);
    connect(timer, SIGNAL(timeout()),this,SLOT(updateLayoutElements()));
    timer->start();
}

/**
 * @brief LayoutManager::loadLayout Loads a saved project layout
 * @param layout
 * @return
 */
bool LayoutManager::loadLayout(QString layout){


    return true;
}

/**
 * @brief LayoutManager::saveLayout Loads a saved project layout
 * @param layout
 * @return
 */
bool LayoutManager::saveLayout(QString layout){



    return true;
}

/**
 * @brief LayoutManager::updateLayoutElements Calls update on a timed basis on all
 *         items in the layout.
 */
void LayoutManager::updateLayoutElements(){
    QGraphicsScene::update();
}
