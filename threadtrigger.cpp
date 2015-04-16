#include "threadtrigger.h"
#include "qdebug.h"

using namespace GPIO;

ThreadTrigger::ThreadTrigger()
{

}

ThreadTrigger::~ThreadTrigger()
{

}



void ThreadTrigger::run()
{

    bool pinlaststate=false;
    qDebug()<<"Trigger thread started";
    GPIOManager* m_gpiomanager=GPIOManager::getInstance();

    int pin = GPIOConst::getInstance()->getGpioByKey("P8_7");

    m_gpiomanager->exportPin(pin);
//    m_gpiomanager->setDirection(pin,GPIO::INPUT);

   // m_gpiomanager->waitForEdge(pin,GPIO::RISING);



    while (!isInterruptionRequested()){
        //m_gpiomanager->waitForEdge(pin,GPIO::RISING);
        if(m_gpiomanager->getValue(pin)==1 && pinlaststate==false){
            pinlaststate=true;
            qDebug()<<"Triggered up";
            emit TriggerReceived();
        }

        if(m_gpiomanager->getValue(pin)==0 && pinlaststate==true){
            pinlaststate=false;
            qDebug()<<"Triggered down";
        }

    }

    delete m_gpiomanager;

}
