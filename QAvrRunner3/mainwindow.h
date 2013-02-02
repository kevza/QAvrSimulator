#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <core/avr_core_builder.h>
#include <stdint.h>
#include <iostream>
#include <QMutex>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QStringList>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    
private slots:
    void on_btnHardware_pressed();

    void on_btnHardware_released();

    void on_loadHex_clicked();

    void gui_update();

    void on_actionStart_triggered();

    void on_actionPause_triggered();

    void on_actionStep_triggered();


protected:
    virtual void closeEvent(QCloseEvent *);

private:
    Avr_Core *core;
    QString rom;
    Ui::MainWindow *ui;
    Avr_Hardware_Interface *hardware;
    Avr_Hardware_Interface *ledmat;
    QGraphicsRectItem* rect[36];
    QGraphicsScene* myScene;
};

#endif // MAINWINDOW_H
