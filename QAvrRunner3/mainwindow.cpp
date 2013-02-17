#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QComboBox>

#include <Tools/buttonitem.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = NULL;
    debugger = new DebugView();
    rom = "";
    connect(ui->actionLoad_Hex,SIGNAL(triggered()),this,SLOT(on_loadHex_clicked()));

    //Setup QGraphics Scene (this is testing code not production)
    myScene = new QGraphicsScene();

    for (int x = 0 ; x < 5; x++){
        for (int y = 0; y < 7;y++){
            rect[y * 5 + x] = new QGraphicsRectItem(10 + (12 * x),10 + (12 * y),10,10);
            rect[y * 5 + x]->setBrush(Qt::black);
            rect[y * 5 + x]->setFlag(QGraphicsItem::ItemIsMovable,true);
            myScene->addItem(rect[y * 5 + x]);
        }
    }
    rect[35] = new QGraphicsRectItem(0,0,10,10);
    rect[35]->setBrush(Qt::black);
    myScene->addItem(rect[35]);


    btn = new ButtonItem();
    btn->setScale(0.2);
    myScene->addItem(btn);

    ui->mainGView->setScene(myScene);
    ui->mainGView->show();
    ui->toolBar->addWidget(new QComboBox());

    this->hardware = NULL;
    this->ledmat = NULL;

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
    if (debugger){
        delete debugger;
    }
    delete ui;
}

/**
  * @brief MainWindow::closeEvent Catches the close event of the window and shuts down gracefully
  * @param event
  */
 void MainWindow::closeEvent(QCloseEvent *event){
    //Stop a running core
    if (core){
        core->isThreadStopped = true;
        while (core->isRunning()){

        }
    }
    //close any open debug windows
    if (debugger){
        debugger->close();
    }
    event->accept();
}


 /**
 * @brief MainWindow::on_actionStart_triggered Starts a new core
 */
void MainWindow::on_actionStart_triggered()
{
    if (ui->actionStart->isChecked()){
        if (QFile(rom).exists()){

            Avr_Core_Builder builder;
            core = builder.loadCore("plugins/ATMega32u2");

            core->isThreadStopped = true;
            core->reg->pc = 0;
            core->flash->loadHex(rom.toStdString().c_str());

            ui->actionPause->setEnabled(true);
            ui->actionPause->setChecked(true);
            ui->actionStep->setEnabled(true);
            //Register the core with the debug window
            if (debugger){
                debugger->attachCore(core);
            }

        }else{
            QMessageBox msgBox;
            msgBox.setText("A valid rom (.hex file) is required!");
            msgBox.exec();
            ui->actionStart->setChecked(false);
        }
    }else{
        core->isThreadStopped = true;
        while (core->isRunning()){
            //Wait for core to finish
        }
        if (core){
            if (debugger){
                debugger->attachCore(NULL);
            }
            delete core;
            core = NULL;
        }
        ui->actionPause->setDisabled(true);
        ui->actionStep->setDisabled(true);

    }


}

/**
 * @brief MainWindow::on_actionPause_triggered Pauses a currently running core.
 */
void MainWindow::on_actionPause_triggered()
{
    //Pausing Core
    if (ui->actionPause->isChecked()){
        if (core){
            this->core->isThreadStopped = true;
        }
        this->ui->actionStep->setEnabled(true);
    }else{
        if (core){
            this->core->isThreadStopped = false;
            this->core->start();
        }
        this->ui->actionStep->setDisabled(true);
    }
}

/**
 * @brief MainWindow::on_actionStep_triggered Step a paused core
 */
void MainWindow::on_actionStep_triggered()
{
    if (core){
        core->step();
    }
}

/**
 * @brief MainWindow::on_loadHex_clicked Selects a hex file to run in the simulator
 */
void MainWindow::on_loadHex_clicked(){
    rom = QFileDialog::getOpenFileName(this, tr("Open Hex"), "~", tr("Hex Files (*.hex)"));
    if (!QFile(rom).exists()){
        rom = "";
    }
}


/**
 * @brief MainWindow::gui_update Trigger an update of all gui elements on
 *          timeout
 */
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
                    btn->setHardware(h);
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

        }
        m.unlock();
    }
}



/**
 * @brief MainWindow::on_actionDebugger_triggered Shows and hides the debug view
 */
void MainWindow::on_actionDebugger_triggered()
{
    if (ui->actionDebugger->isChecked()){
        //Show the debugger
        if (!debugger){
            debugger = new DebugView(NULL);

        }
        debugger->show();
        debugger->attachCore(this->core);
    }else{
        //Hide the debugger
        debugger->hide();

    }
}
