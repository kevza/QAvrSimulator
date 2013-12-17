#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include <QWidget>
#include <QMutex>
#include <QTimer>
#include <core/core.h>

namespace Ui {
class DebugView;
}

class DebugView : public QWidget
{
    Q_OBJECT
    
public:
    explicit DebugView(QWidget *parent = 0);
    ~DebugView();
    void attachCore(Avr_Core *core);

private:
    Ui::DebugView *ui;
    Avr_Core *core;

private slots:
    void update();


};

#endif // DEBUGVIEW_H
