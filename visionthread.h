#ifndef VISIONTHREAD_H
#define VISIONTHREAD_H

#include <QObject>
#include "QThread"
#include "GPIO/GPIOManager.h"
#include "GPIO/GPIOConst.h"
#include "visionprocessing.h"


class VisionThread : public QThread
{
    Q_OBJECT
public:
    VisionThread(VisionProcessing* vision);
    ~VisionThread();

    // QThread interface
protected:
    void run();

private:
    VisionProcessing* m_vision;
signals:
 //void TriggerReceived();

};



#endif // THREADTRIGGER_H
