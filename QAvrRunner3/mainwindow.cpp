#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QComboBox>


#include <Scene/layoutmanager.h>


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
    myScene = new LayoutManager();

    //Add the buttons to the controller
    int xPos[] = {80,140,140,140,200,260};
    int yPos[] = {60,0,60,120,60,120};
    QString ports[] = {"PINC6","PINC7","PINC4","PINC5","PINB7","PIND7"};
    bool setting[] = {true,true,true,true,true,false};
    for (int i = 0; i < 6; i++){
        btn[i] = new ButtonItem();
        btn[i]->setScale(0.2);
        btn[i]->setX(xPos[i]);btn[i]->setY(yPos[i]);
        btn[i]->setPin(ports[i]);
        btn[i]->setPushLow(setting[i]);
        myScene->addItem(btn[i]);
    }

    //Add the ledmat to the controller
    ledMatItem = new LedMatItem();
    myScene->addItem(ledMatItem);

    ui->mainGView->setScene(myScene);
    ui->mainGView->show();

    //Load a list of available cores
    coresList = new QComboBox();
    QDir d;

    //Set the plugin directory
    d.setCurrent("./plugins");

    QStringList filter;
    filter << "*.def";
    coresList->addItems(d.entryList(filter,QDir::Files));
    ui->toolBar->addWidget(coresList);
    connect(coresList,SIGNAL(currentIndexChanged(QString)),this,SLOT(core_changed(QString)));
    if (coresList->count() > 0){
        this->coreDef = coresList->itemText(0);
    }

    this->hardware = NULL;
    this->ledmat = NULL;

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
    if (debugger){
        delete debugger;
    }
    delete coresList;
    delete ui;
}

/**
 * @brief MainWindow::core_changed Slot for changes in core from combo box
 * @param def
 */
void MainWindow::core_changed(QString def){
    coreDef = def;
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
            if (QFile(coreDef).exists()){
                Avr_Core_Builder builder;
                core = builder.loadCore(coreDef);

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
                msgBox.setText("Please select a valid core!");
                msgBox.exec();
                ui->actionStart->setChecked(false);
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
    static bool setup = false;
    if (core){
        if (!setup){
            qDebug() << "Im not Timed in";
            foreach (Avr_Hardware_Interface * h ,core->hardware){
                if (h->getPluginName() == "AVRIO"){
                    this->hardware = h;
                    for (int i = 0 ; i < 6; i++){
                        btn[i]->connectHardware(h);
                    }
                }
                if (h->getPluginName() == "AVRLEDMAT"){
                    ledMatItem->connectHardware(h);
                }
            }
            setup = true;
        }else{
           //this->ui->mainGView->repaint();
            ledMatItem->update();
        }
    }
}



/**
 * @brief MainWindow::on_actionDebugger_triggered Shows and hides the debug view
 */
void MainWindow::on_actionDebugger_triggered()
{
    if (!debugger || debugger->isHidden()){
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
