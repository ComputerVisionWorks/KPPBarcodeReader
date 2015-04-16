#include "visionthread.h"
#include "qdebug.h"

using namespace GPIO;

VisionThread::VisionThread(VisionProcessing *vision)
{
    m_vision=vision;

}

VisionThread::~VisionThread()
{

}



void VisionThread::run()
{




    qDebug()<<"Vision thread started";
    /*
    bool pinlaststate=false;
    GPIOManager* m_gpiomanager=GPIOManager::getInstance();

    int pin = GPIOConst::getInstance()->getGpioByKey("P8_7");

    m_gpiomanager->exportPin(pin);
//    m_gpiomanager->setDirection(pin,GPIO::INPUT);

   // m_gpiomanager->waitForEdge(pin,GPIO::RISING);

*/
    msleep(500);

    while (!isInterruptionRequested()){


        m_vision->Capture(5);

        msleep(10);
    }


/*

        //m_gpiomanager->waitForEdge(pin,GPIO::RISING);
        if(m_gpiomanager->getValue(pin)==1 && pinlaststate==false){
            pinlaststate=true;
            qDebug()<<"Triggered up";
            //emit TriggerReceived();
        }

        if(m_gpiomanager->getValue(pin)==0 && pinlaststate==true){
            pinlaststate=false;
            qDebug()<<"Triggered down";
        }

    delete m_gpiomanager;
    */


}
