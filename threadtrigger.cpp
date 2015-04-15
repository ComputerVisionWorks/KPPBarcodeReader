#include "threadtrigger.h"

using namespace GPIO;

ThreadTrigger::ThreadTrigger()
{

}

ThreadTrigger::~ThreadTrigger()
{

}



void ThreadTrigger::run()
{

    GPIOManager* m_gpiomanager=GPIOManager::getInstance();

    int pin = GPIOConst::getInstance()->getGpioByKey("P8_10");

    m_gpiomanager->exportPin(pin);
    m_gpiomanager->setDirection(pin,GPIO::INPUT);

    m_gpiomanager->waitForEdge(pin,GPIO::RISING);


    while (!isInterruptionRequested()){
        m_gpiomanager->waitForEdge(pin,GPIO::RISING);
    }

    delete m_gpiomanager;

}
