#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <QGraphicsScene>

class LayoutManager : public QGraphicsScene
{
    public:
        /**
         * @brief LayoutManager
         */
        LayoutManager(QObject *parent = 0);
        /**
         * @brief loadLayout
         * @param layout
         * @return
         */
        bool loadLayout(QString layout);
        bool saveLayout(QString layout);

    private slots:
        void updateLayoutElements();
};

#endif // SCENEMANAGER_H
