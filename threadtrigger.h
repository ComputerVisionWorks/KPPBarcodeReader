#ifndef THREADTRIGGER_H
#define THREADTRIGGER_H

#include <QObject>
#include "QThread"
#include "GPIO/GPIOManager.h"
#include "GPIO/GPIOConst.h"

class ThreadTrigger : public QThread
{
    Q_OBJECT
public:
    ThreadTrigger();
    ~ThreadTrigger();

    // QThread interface
protected:
    void run();

signals:
 void TriggerReceived();

};



#endif // THREADTRIGGER_H
