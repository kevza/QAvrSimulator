#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = NULL;
    connect(ui->actionLoad_Hex,SIGNAL(triggered()),this,SLOT(on_loadHex_clicked()));


    myScene = new QGraphicsScene();
    for (int x = 0 ; x < 5; x++){
        for (int y = 0; y < 7;y++){
            rect[y * 5 + x] = new QGraphicsRectItem(10 + (12 * x),10 + (12 * y),10,10);



            rect[y * 5 + x]->setBrush(Qt::black);

            myScene->addItem(rect[y * 5 + x]);
        }
    }
    rect[35] = new QGraphicsRectItem(0,0,10,10);
    rect[35]->setBrush(Qt::black);
    myScene->addItem(rect[35]);

    ui->mainGView->setScene(myScene);
    ui->mainGView->show();
    QTimer *timer = new QTimer;
    timer->setInterval(10);
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


    if (core){
        if (ui->actionDebug_Mode->isChecked()){
            core->debug = true;
        }else{
            core->debug = false;
        }
        QMutex m;
        m.lock();
        if (!this->hardware || !ledmat){

            foreach (Avr_Hardware_Interface * h ,core->hardware){
                if (h->getPluginName() == "AVRIO"){

                    this->hardware = h;
                }
                if (h->getPluginName() == "AVRLEDMAT"){

                    ledmat = h;
                }
            }
        }else{


            QMap<QString,uint8_t> theMap;
            theMap = ledmat->getOutputs();
            QStringList leds;
            leds  << "C4R0"<< "C3R0"<< "C2R0"<< "C1R0"<< "C0R0"
                  << "C4R1"<< "C3R1"<< "C2R1"<< "C1R1"<< "C0R1"
                  << "C4R2"<< "C3R2"<< "C2R2"<< "C1R2"<< "C0R2"
                  << "C4R3"<< "C3R3"<< "C2R3"<< "C1R3"<< "C0R3"
                  << "C4R4"<< "C3R4"<< "C2R4"<< "C1R4"<< "C0R4"
                  << "C4R5"<< "C3R5"<< "C2R5"<< "C1R5"<< "C0R5"
                  << "C4R6"<< "C3R6"<< "C2R6"<< "C1R6"<< "C0R6";


            for (int x = 0 ; x < 5; x++){
                for (int y = 0; y < 7;y++){

                    rect[y * 5 + x]->setBrush(QBrush(QColor(theMap[leds.at(y * 5 + x)],0,0)));

                }
            }

            QMap <QString, uint8_t> myMap = hardware->getOutputs();
            rect[35]->setBrush(QBrush(QColor(myMap["PORTC2"],0,0)));

            myScene->update();


        /*
        QMap <QString, uint8_t> myMap;

        myMap = hardware->getOutputs();
        std::cout << t << " : ";
        std::cout << (int)myMap["PORTB0"] << ", ";
        std::cout << (int)myMap["PORTB1"] << ", ";
        std::cout << (int)myMap["PORTB2"] << ", ";
        std::cout << (int)myMap["PORTB3"] << ", ";
        std::cout << (int)myMap["PORTB4"] << ", ";
        std::cout << (int)myMap["PORTB5"] << ", ";
        std::cout << (int)myMap["PORTB6"] << ", ";
        std::cout << (int)myMap["PORTB7"] << ",";
        std::cout <<"-";
        std::cout << (int)myMap["PORTC0"] << ", ";
        std::cout << (int)myMap["PORTC1"] << ", ";
        std::cout << (int)myMap["PORTC2"] << ", ";
        std::cout << (int)myMap["PORTC3"] << ", ";
        std::cout << (int)myMap["PORTC4"] << ", ";
        std::cout << (int)myMap["PORTC5"] << ", ";
        std::cout << (int)myMap["PORTC6"] << ", ";
        std::cout << (int)myMap["PORTC7"] << ",";
        std::cout <<"-";
        std::cout << (int)myMap["PORTD0"] << ", ";
        std::cout << (int)myMap["PORTD1"] << ", ";
        std::cout << (int)myMap["PORTD2"] << ", ";
        std::cout << (int)myMap["PORTD3"] << ", ";
        std::cout << (int)myMap["PORTD4"] << ", ";
        std::cout << (int)myMap["PORTD5"] << ", ";
        std::cout << (int)myMap["PORTD6"] << ", ";
        std::cout << (int)myMap["PORTD7"] << "\n";
        t++;*/
        }
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
    //QString fileName = "/home/kevin/Documents/Uni Stuff/Second Year/Second Year S2/ENCE260/Embedded/ucfk4/lab_1/ex1/ex1.hex";//QFileDialog::getOpenFileName(this, tr("Open Hex"), "~", tr("Hex Files (*.hex)"));
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
