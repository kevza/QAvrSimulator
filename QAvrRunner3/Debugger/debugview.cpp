#include "debugview.h"
#include "ui_debugview.h"
#include <string>

DebugView::DebugView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugView)
{
    ui->setupUi(this);
    this->core = NULL;
    QTimer *time = new QTimer;

    time->setInterval(20);
    connect(time,SIGNAL(timeout()),this,SLOT(update()));
    time->start();

    //Setup Lists in Registers View
    for (int i = 0 ; i < 32; i++){
        ui->registersWidget->addItem(new QListWidgetItem(QString::number(i,16)));
    }
    for (int i = 32 ; i < 0xff; i++){
        ui->ioWidget->addItem(new QListWidgetItem(QString::number(i,16)));
    }
}

DebugView::~DebugView()
{
    delete ui;
}


void DebugView::attachCore(Avr_Core *core){
    this->core = core;
}

/**
 * @brief DebugView::update Updates the processor state views
 */
void DebugView::update(){

    if (core){
        //Get debug log
        string log;
        QMutex mutex;
        mutex.lock();
        //Get instructions from the debug Queue
        while (core->debugQueue.size() > 0){
            log = core->debugQueue.front();
            core->debugQueue.pop();
            ui->assemblyTraceTxt->appendPlainText(QString(log.c_str()));
        }
        //Update Registers in Window
        for (int i = 0 ; i < 32; i++){
            QListWidgetItem *item =  ui->registersWidget->item(i);
            item->setText("R:"+ QString::number(i,16) + " = " + QString::number((int)core->reg->ram[i]));
        }
        //Check input output registers
        if (0xff <= core->mem->getRamEnd()){
            for (int i = 0 ; i < (0xff - 0x20); i++){
                QListWidgetItem *item = ui->ioWidget->item(i);
                item->setText("R:"+ QString::number(i + 0x20,16) + " = " + QString::number((int)core->reg->ram[i + 0x20]));
            }
        }
        //Get the stack state
        int stackLen = core->mem->getRamEnd() - core->reg->sp(0);
        if (stackLen > 0 && stackLen != core->mem->getRamEnd()){
            for (int i = 0; i < stackLen;i++){
                if (i < ui->currentStackWidget->count()){
                    //Item in list
                    QListWidgetItem *item = ui->currentStackWidget->item(i);
                    item->setText(QString::number((int)core->reg->ram[core->mem->getRamEnd() - i - 1]));
                }else{
                    //Add new Item to list
                    ui->currentStackWidget->addItem(new QListWidgetItem(QString::number((int)core->reg->ram[core->mem->getRamEnd() - i - 1])));
                }
            }
            if (stackLen < ui->currentStackWidget->count()){
                for (int i = ui->currentStackWidget->count() - 1; i >= stackLen; i--){
                    ui->currentStackWidget->item(i)->setText("");
                }
            }

        }
        mutex.unlock();
    }
}
