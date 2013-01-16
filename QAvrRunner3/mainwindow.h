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
    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_btnStep_clicked();


    void on_btnHardware_pressed();

    void on_btnHardware_released();

    void on_loadHex_clicked();

    void gui_update();

private:
    Avr_Core *core;
    Ui::MainWindow *ui;
    Avr_Hardware_Interface *hardware;
    Avr_Hardware_Interface *ledmat;
    QGraphicsRectItem* rect[35];
    QGraphicsScene* myScene;
};

#endif // MAINWINDOW_H
