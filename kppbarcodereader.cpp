#include "kppbarcodereader.h"


using namespace GPIO;



KPPBarcodeReader::KPPBarcodeReader(QObject *parent, QGraphicsView *viewer) :
    QObject(parent)
{

    timer_getImage= new QTimer(this);
    timer_getImage->setSingleShot(true);
    timer_getImage->setInterval(50);
    connect(timer_getImage, SIGNAL(timeout()),this, SLOT(Capture()));
    //timer_getImage->start();
    m_visionprocessing = new VisionProcessing(this);

    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);
    m_decodeInterval=50;
    m_decodeType=OneShotGoodRead;
    m_trigger=new ThreadTrigger();
    connect(m_trigger, SIGNAL(finished()), m_trigger, SLOT(deleteLater()));
    connect(m_trigger, SIGNAL(TriggerReceived()), this, SLOT(Capture()),Qt::DirectConnection);

    cvcamera=0;
    m_captureEnabled=false;
    m_CapturedPixmap=0;
    m_viewer=viewer;
    if(viewer!=0){
        scene_processed = new QGraphicsScene();
        m_CapturedPixmap=scene_processed->addPixmap(QPixmap());
        m_viewer->setScene(scene_processed);
    }

    cvcamera= new VideoCapture(-1);

    m_gpiomanager=GPIOManager::getInstance();

    m_LedsPin = GPIOConst::getInstance()->getGpioByKey("P8_13");

    m_gpiomanager->exportPin(m_LedsPin);
    m_gpiomanager->setDirection(m_LedsPin,GPIO::OUTPUT);


}


KPPBarcodeReader::~KPPBarcodeReader()
{
    timer_getImage->stop();
    delete cvcamera;
    delete decoder;
    delete m_visionprocessing;
    delete m_gpiomanager;
}



void KPPBarcodeReader::EnableDecoding()
{


}

void KPPBarcodeReader::DisableDecoding()
{


}


bool KPPBarcodeReader::captureEnabled() const
{
    return m_captureEnabled;
}

void KPPBarcodeReader::setCaptureEnabled(bool captureEnabled)
{
    m_captureEnabled = captureEnabled;
    if(captureEnabled){
        timer_getImage->start();
    }
    else
        timer_getImage->stop();
}
bool KPPBarcodeReader::UseTrigger() const
{
    return m_UseTrigger;
}

void KPPBarcodeReader::setUseTrigger(bool UseTrigger)
{
    m_UseTrigger = UseTrigger;

    if(m_trigger->isRunning())
        setUseTrigger(false);

    if (UseTrigger) {
        setDecodeType(OneShotGoodRead);
        m_trigger->start();


    }
    else {
        m_trigger->requestInterruption();
        m_trigger->wait();
    }
}




void KPPBarcodeReader::setCamera(int Index)
{

    //if(cvcamera->isOpened()){
    //     cvcamera->release();
    //  }

    cvcamera->open(Index);


    if(cvcamera->isOpened()){
        cvcamera->set(CV_CAP_PROP_FRAME_WIDTH,640);
        cvcamera->set(CV_CAP_PROP_FRAME_HEIGHT,480);



        cvcamera->set(CV_CAP_PROP_FPS,15);
    }

}

void KPPBarcodeReader::CloseCamera()
{
    StopCapture();
    cvcamera->release();
}

void KPPBarcodeReader::StopCapture()
{
 timer_getImage->stop();
}




void KPPBarcodeReader::Capture(int frames)
{
    QList<QString> barcodes;
    try
    {
        if(!cvcamera->isOpened()) return;

        m_gpiomanager->setValue(m_LedsPin,GPIO::HIGH);
        Mat frame;
        int i;
        for(i=0;i<frames;i++){
           bool ret= cvcamera->read(frame); // get a new frame from camera
           if(ret==false){
               cvcamera->release();
               return;
           }
        }
        barcodes=m_visionprocessing->getBarcodeFromImage(frame,decoder,m_CapturedPixmap);
        if(barcodes.count()>0){
            emit BarcodesFound(barcodes);
            m_gpiomanager->setValue(m_LedsPin,GPIO::LOW);
        }
        m_viewer->fitInView(m_CapturedPixmap->boundingRect() ,Qt::KeepAspectRatio);


    }
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;

    }
    if(decodeType()!=OneShot && (decodeType()==OneShotGoodRead && barcodes.count()==0))
        timer_getImage->start();

   // m_gpiomanager->setValue(m_LedsPin,GPIO::LOW);
}



KPPBarcodeReader::DecodeType KPPBarcodeReader::decodeType() const
{
    return m_decodeType;
}

void KPPBarcodeReader::setDecodeType(const DecodeType &decodeType)
{
    m_decodeType = decodeType;
}

int KPPBarcodeReader::decodeInterval() const
{
    return m_decodeInterval;
}

void KPPBarcodeReader::setDecodeInterval(int decodeInterval)
{
    if(decodeInterval>50)
        if(m_decodeInterval != decodeInterval){
            m_decodeInterval = decodeInterval;
            timer_getImage->setInterval(decodeInterval);

        }

}

VisionProcessing *KPPBarcodeReader::getVisionProcessing() const
{
    return m_visionprocessing;
}

QList<QString> KPPBarcodeReader::AvaibleCameras()
{
    QList<QString> cameras;

    //for(int i=0;i<QCameraInfo::availableCameras().count();i++){
    //      cameras.append(QCameraInfo::availableCameras().at(i).description());
    //  }

    return cameras;

}


