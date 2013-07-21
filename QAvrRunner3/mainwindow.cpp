#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QComboBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtWebKit/QWebView>
#include <QDesktopServices>


#include <Workbench/layoutmanager.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->buildMenus();
    core = NULL;
    debugger = new DebugView();
    rom = "";
    connect(ui->actionLoad_Hex,SIGNAL(triggered()),this,SLOT(on_loadHex_clicked()));

    //Setup QGraphics Scene (this is testing code not production)
    myScene = new LayoutManager();
    //Add the buttons to the controller
    int xPos[] = {80,140,140,140,200,260};
    int yPos[] = {60,0,60,120,60,120};
    int keyCodes[] = {52,56,53,50,54,Qt::Key_Enter};
    QString ports[] = {"PINB7","PINC7","PINC4","PINC5","PINC6","PIND7"};
    bool setting[] = {true,true,true,true,true,false};
    for (int i = 0; i < 6; i++){
        btn[i] = new ButtonItem();
        btn[i]->setScale(0.2);
        btn[i]->setX(xPos[i]);btn[i]->setY(yPos[i]);
        btn[i]->setPin(ports[i]);
        btn[i]->setPushLow(setting[i]);
        btn[i]->bindKey(keyCodes[i]);
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
    this->setWindowIcon(QIcon(":/icons/Icons/MainWindow.png"));
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
    delete portMenu;
    delete actionGroup;
    delete actionGroup2;
    delete baudMenu;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    for (int i = 0 ; i < 6 ; i++)
        btn[i]->keyPressEvent(event);
    QMainWindow::keyPressEvent(event) ;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){
    for (int i = 0 ; i < 6 ;i ++)
        btn[i]->keyReleaseEvent(event);
    QMainWindow::keyReleaseEvent(event) ;
}


void MainWindow::buildMenus(){
    //Create Serial Ports Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    portMenu = new QMenu("Serial Ports", this);
    QSerialPortInfo ports;
    foreach (QSerialPortInfo p, ports.availablePorts()){
        QAction *portAction = new QAction(this);
        portAction->setText(p.description());
        portAction->setCheckable(true);
        portAction->setData(p.systemLocation());
        this->serialPortActions.append(portAction);
        actionGroup->addAction(portAction);
        portMenu->addAction(portAction);
    }
    QAction *portAction = new QAction(this);
    portAction->setText(tr("/dev/pts/0"));
    portAction->setCheckable(true);
    portAction->setData(tr("/dev/pts/0"));
    actionGroup->addAction(portAction);
    portMenu->addAction(portAction);
    this->serialPortActions.append(portAction);
    portAction = new QAction(this);
    portAction->setText(tr("/dev/pts/1"));
    portAction->setCheckable(true);
    portAction->setData(tr("/dev/pts/1"));
    actionGroup->addAction(portAction);
    portMenu->addAction(portAction);
    this->serialPortActions.append(portAction);
    portAction = new QAction(this);
    portAction->setText(tr("/dev/pts/2"));
    portAction->setCheckable(true);
    portAction->setData(tr("/dev/pts/2"));
    actionGroup->addAction(portAction);
    portMenu->addAction(portAction);
    this->serialPortActions.append(portAction);
    portAction = new QAction(this);
    portAction->setText(tr("/dev/pts/3"));
    portAction->setCheckable(true);
    portAction->setData(tr("/dev/pts/3"));
    actionGroup->addAction(portAction);
    portMenu->addAction(portAction);
    this->serialPortActions.append(portAction);
    portAction = new QAction(this);
    portAction->setText(tr("/dev/pts/4"));
    portAction->setCheckable(true);
    portAction->setData(tr("/dev/pts/4"));
    actionGroup->addAction(portAction);
    portMenu->addAction(portAction);
    this->serialPortActions.append(portAction);
    ui->menuSerial->addMenu(portMenu);

    //Create Baud Rate Menu
    QStringList baud = QStringList() << "2400" << "4800" << "9600" << "19200" << "38400"
                        << "57600" << "115200";
    actionGroup2 = new QActionGroup(this);
    actionGroup2->setExclusive(true);
    baudMenu = new QMenu("Baud Rate", this);
    foreach(QString b, baud){
        QAction *baudAction = new QAction(this);
        baudAction->setText(b);
        baudAction->setCheckable(true);
        baudMenu->addAction(baudAction);
        this->baudActions.append(baudAction);
        actionGroup2->addAction(baudAction);
    }
    ui->menuSerial->addMenu(baudMenu);

    //Create Refresh Menu
    QAction *refresh = new QAction(this);
    refresh->setText("Refresh Ports");
    connect(refresh,SIGNAL(triggered()),this,SLOT(refresh_menus()));
    ui->menuSerial->addSeparator();
    ui->menuSerial->addAction(refresh);

    //Create Software Echo Option
    sEcho = new QAction(this);
    sEcho->setText("Software Echo");
    sEcho->setCheckable(true);
    sEcho->setChecked(true);

    ui->menuSerial->addAction(sEcho);

    hEcho = new QAction(this);
    hEcho->setText("Hardware Echo");
    hEcho->setCheckable(true);
    hEcho->setChecked(false);

    ui->menuSerial->addAction(hEcho);


}

void MainWindow::refresh_menus(){
    //Clean Menu
    foreach (QAction *action, portMenu->actions()){
        portMenu->removeAction(action);
        this->serialPortActions.removeOne(action);
        delete action;
    }
    QSerialPortInfo ports;
    foreach (QSerialPortInfo p, ports.availablePorts()){
        QAction *portAction = new QAction(this);
        portAction->setText(p.description());
        portAction->setCheckable(true);
        portAction->setData(p.systemLocation());
        this->serialPortActions.append(portAction);
        actionGroup->addAction(portAction);
        portMenu->addAction(portAction);
    }
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

                //Set Menu Bar
                ui->actionPause->setEnabled(true);
                ui->actionPause->setChecked(true);
                ui->actionStep->setEnabled(true);

                //Set Baud Rate on Serial Device
                foreach (Avr_Hardware_Interface * h ,core->hardware){
                    if (h->getPluginName() == "AVRUART"){
                        QString port, baud;
                        foreach(QAction *action, serialPortActions){
                            if (action->isChecked()){
                                port = action->data().toString();
                                qDebug() << "Port : " << port;
                            }
                        }
                        foreach(QAction *action, baudActions){
                            if(action->isChecked()){
                                baud = action->text();
                            }
                        }
                        QMap <QString,QString> setting;
                        setting["BAUD"] = baud;
                        setting["PORT"] = port;
                        if (sEcho->isChecked()){
                            setting["SECHO"] = "True";
                        }else{
                            setting["SECHO"] = "False";
                        }

                        if (hEcho->isChecked()){
                            setting["HECHO"] = "True";
                        }else{
                            setting["HECHO"] = "False";
                        }
                        h->passSetting(setting);
                    }
                }



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

                if (h->getPluginName() == "AVRUART"){

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

/**
  * @brief MainWindow::on_actionAbout_QAvr_Simulator_triggered() Shows the about dialog for QAvrSimulator
  */
void MainWindow::on_actionAbout_QAvr_Simulator_triggered()
{
    QFile docFile(":/Docs/Docs/About.md");
    if (!docFile.open(QIODevice::ReadOnly)){
        qDebug() << "About resource failed to open";
        return;
    }
    QTextStream in(&docFile);
    QString text = QString(in.readAll());
    QMessageBox::about(this,"About QAvrSimulator",text);
}

/**
  *@brief MainWindows::on_actionAbout_QT_triggered() Displays information about QT version in use.
  */
void MainWindow::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this);
}

/**
  * @brief MainWindows::on_actionHelp_triggered() Shows help dialog
  */
void MainWindow::on_actionHelp_triggered()
{
    QWebView *helpView = new QWebView(this);
    helpView->setWindowTitle(tr("QAvrSimulator Help"));
    helpView->load(QUrl("qrc:/Help/Help/index.html"));
    helpView->setWindowFlags(Qt::Dialog);
    helpView->show();
}

void MainWindow::on_actionE_xit_triggered()
{
   this->close();
}

void MainWindow::on_action_Report_a_Bug_triggered()
{
    QDesktopServices::openUrl(QUrl("mailto:kevzawinning@gmail.com?subject=QAvrBug Report&body=What Happened :....\n\nWhat I Expected to Happen :....\n\nNote: Please Attach the Hex file you were running, the source code that generated the Hex file is also helpful."));
}
