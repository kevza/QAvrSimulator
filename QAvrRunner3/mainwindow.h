#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <core/avr_core_builder.h>
#include <stdint.h>
#include <iostream>


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


private:
    Avr_Core *core;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
