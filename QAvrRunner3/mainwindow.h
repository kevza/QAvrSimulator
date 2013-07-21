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
#include <QComboBox>
#include <QActionGroup>
#include <QAction>
#include <QtSerialPort/QSerialPortInfo>
#include <Workbench/Tools/buttonitem.h>
#include <Workbench/Tools/ledmatitem.h>
#include <Debugger/debugview.h>

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

    void on_loadHex_clicked();

    void gui_update();

    void on_actionStart_triggered();

    void on_actionPause_triggered();

    void on_actionStep_triggered();

    void on_actionDebugger_triggered();

    void core_changed(QString def);

    void refresh_menus();

    void on_actionAbout_QAvr_Simulator_triggered();

    void on_actionAbout_QT_triggered();

    void on_actionHelp_triggered();

    void on_actionE_xit_triggered();

    void on_action_Report_a_Bug_triggered();

protected:
    virtual void closeEvent(QCloseEvent *);

private:
    Avr_Core *core;
    QString rom;
    QString coreDef;
    QComboBox *coresList;
    Ui::MainWindow *ui;
    Avr_Hardware_Interface *hardware;
    Avr_Hardware_Interface *ledmat;
    QGraphicsRectItem* rect[36];
    QGraphicsScene* workbench;
    ButtonItem *btn[6];
    LedMatItem *ledMatItem;
    DebugView *debugger;

    //Menu Groups
    //Serial Port
    QActionGroup *actionGroup;
    QMenu *portMenu;
    QList <QAction*> serialPortActions;
    QList <QSerialPortInfo> serialPortInfo;

    QActionGroup *actionGroup2;
    QList <QAction*> baudActions;
    QMenu *baudMenu;

    //Serial Echo Settings
    QAction *sEcho;
    QAction *hEcho;

    //Build Menus
    void buildMenus();

    //Handle Key Press Events
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
