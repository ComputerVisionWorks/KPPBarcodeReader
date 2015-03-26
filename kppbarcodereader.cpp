#include "kppbarcodereader.h"






KPPBarcodeReader::KPPBarcodeReader(QObject *parent) :
    QObject(parent)
{
    m_Capture=new CameraCapture(this);  
    timer_getImage= new QTimer(this);
    timer_getImage->setSingleShot(true);
    timer_getImage->setInterval(50);
    connect(timer_getImage, SIGNAL(timeout()),m_Capture, SLOT(CaptureImage()));
    m_visionprocessing = new VisionProcessing(this);
    connect(m_Capture,SIGNAL(ImageAvaible(QImage)),this,SLOT(ImageAvaible(QImage)));

    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);
    m_decodeInterval=50;
    m_decodeType=Continuous;
}


KPPBarcodeReader::~KPPBarcodeReader()
{
    delete m_Capture;
    delete decoder;
}
CameraCapture *KPPBarcodeReader::Capture() const
{
    return m_Capture;
}

void KPPBarcodeReader::StartDecoding()
{


 //timer_getImage->stop();
 timer_getImage->start(m_decodeInterval);

}

void KPPBarcodeReader::StopDecoding()
{
    Capture()->stopCamera();
    timer_getImage->stop();
}

void KPPBarcodeReader::ImageAvaible(QImage img)
{

    QList<QString> tags=m_visionprocessing->getBarcodeFromImage(img,decoder);





    emit BarcodesFound(tags);

    if((m_decodeType==DecodeType::Continuous)){
        timer_getImage->start();
    }
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

    for(int i=0;i<QCameraInfo::availableCameras().count();i++){
        cameras.append(QCameraInfo::availableCameras().at(i).description());
    }

    return cameras;

}


