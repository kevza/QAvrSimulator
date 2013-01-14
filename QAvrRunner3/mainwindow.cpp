#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = NULL;
    connect(ui->actionLoad_Hex,SIGNAL(triggered()),this,SLOT(on_loadHex_clicked()));


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
    QTimer *timer = new QTimer;
    timer->setInterval(20);
    connect(timer,SIGNAL(timeout()),this,SLOT(gui_update()));
    timer->start();
}

MainWindow::~MainWindow()
{
    if (core){
        delete core;
    }
    delete ui;
}

void MainWindow::gui_update(){
    static int t = 0;
    if (core){
        QMutex m;
        m.lock();
        if (!hardware){
            foreach (Avr_Hardware_Interface * h ,core->hardware){
                if (h->getPluginName() == "AVRIO"){
                    std::cout << "Found\n";
                    hardware = h;
                }
            }
        }

        QMap <QString, uint8_t> myMap;

        myMap = hardware->getOutputs();
        std::cout << t << " : ";
        std::cout << (int)myMap["PORTC0"] << ", ";
        std::cout << (int)myMap["PORTC1"] << ", ";
        std::cout << (int)myMap["PORTC2"] << ", ";
        std::cout << (int)myMap["PORTC3"] << ", ";
        std::cout << (int)myMap["PORTC4"] << ", ";
        std::cout << (int)myMap["PORTC5"] << ", ";
        std::cout << (int)myMap["PORTC6"] << ", ";
        std::cout << (int)myMap["PORTC7"] << "\n";
        t++;
        m.unlock();
    }
}

void MainWindow::on_btnStart_clicked()
{
    if (!core){
        Avr_Core_Builder builder;
        core = builder.loadCore("plugins/ATMega32u2");
        //builder.loadProgram("ex1.hex");
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

void MainWindow::on_btnHardware_pressed()
{
    if (core){
        QMutex m;
        m.lock();
        *hardware->getInputs()["PIND"] |= (1 << 7);
        m.unlock();
    }
}

void MainWindow::on_btnHardware_released()
{
    if (core){
        QMutex m;
        m.lock();
        *hardware->getInputs()["PIND"] &= ~(1 << 7);
        m.unlock();
    }
}

void MainWindow::on_loadHex_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Hex"), "~", tr("Hex Files (*.hex)"));
    if (core){
        //Stop any threads
        core->isThreadStopped = true;
        core->reg->pc = 0;
        core->flash->loadHex(fileName.toStdString().c_str());
    }else{
        QMessageBox msgBox;
        msgBox.setText("There must be a valid core!");
        msgBox.exec();
    }
}
