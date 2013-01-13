#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QGraphicsScene>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = NULL;

    QGraphicsScene* myScene = new QGraphicsScene();
    for (int x = 0 ; x < 7; x++){
        for (int y = 0; y < 8;y++){
            QGraphicsRectItem* rect = new QGraphicsRectItem(10 + (12 * x),10 + (12 * y),10,10);



            rect->setBrush(Qt::black);

            myScene->addItem(rect);
        }
    }
    ui->mainGView->setScene(myScene);
    ui->mainGView->show();

}

MainWindow::~MainWindow()
{
    if (core){
        delete core;
    }
    delete ui;
}


void MainWindow::on_btnStart_clicked()
{
    if (!core){
        Avr_Core_Builder builder;
        core = builder.loadCore("plugins/ATMega32u2");
        builder.loadProgram("ex1.hex");
        ui->btnStart->setText("Start Core");
    }else{
        core->start();
    }
}

void MainWindow::on_btnStop_clicked()
{
    core->isThreadStopped = true;
}

void MainWindow::on_btnStep_clicked()
{
    if (core){
        core->step();
    }
}
