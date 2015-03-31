#include "kppbarcodereader.h"
#include "QCameraViewfinder"





KPPBarcodeReader::KPPBarcodeReader(QObject *parent, QGraphicsView *viewer) :
    QObject(parent)
{
    timer_getImage= new QTimer(this);
    timer_getImage->setSingleShot(true);
    timer_getImage->setInterval(20);
    connect(timer_getImage, SIGNAL(timeout()),this, SLOT(CaptureTimer()));
    timer_getImage->start();
    m_visionprocessing = new VisionProcessing(this);

    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);
    m_decodeInterval=50;
    m_decodeType=Continuous;


    m_CameraCapture=0;
    m_SelectedCamera=0;
    m_captureEnabled=false;
    m_CapturedPixmap=0;
    m_viewer=viewer;
    if(viewer!=0){
        scene_processed = new QGraphicsScene();
        m_CapturedPixmap=scene_processed->addPixmap(QPixmap());
        m_viewer->setScene(scene_processed);
    }

}


KPPBarcodeReader::~KPPBarcodeReader()
{
    timer_getImage->stop();
    if(m_SelectedCamera!=0)
        m_SelectedCamera->stop();
    delete m_CameraCapture;
    delete m_SelectedCamera;
    delete decoder;
}



void KPPBarcodeReader::EnableDecoding()
{


}

void KPPBarcodeReader::DisableDecoding()
{


}


QCamera *KPPBarcodeReader::SelectedCamera() const{



    return m_SelectedCamera;
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


void KPPBarcodeReader::ImageAvaible(int, QImage img)
{


     QList<QString> tags=m_visionprocessing->getBarcodeFromImage(img,decoder,m_CapturedPixmap);



         m_viewer->fitInView(m_CapturedPixmap->boundingRect() ,Qt::KeepAspectRatio);


   // emit BarcodesFound(tags);

    if((m_decodeType==KPPBarcodeReader::Continuous)){
        DisableDecoding();
    }

}


void KPPBarcodeReader::setCamera(QCameraInfo *cameraInfo)
{
    if(cameraInfo==m_cameraInfo){
        return;
    }
    //m_SelectedCamera->deviceDescription()
    delete m_CameraCapture;
    delete m_SelectedCamera;
    //m_ImageCapture=0;
    //m_SelectedCamera=0;

    m_cameraInfo=cameraInfo;
    m_SelectedCamera = new QCamera(*cameraInfo);


     m_CameraCapture = new QCameraImageCapture(m_SelectedCamera);



    //connect(m_CameraCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(ImageAvaible(int,QImage)));

    //connect(m_ImageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this,
          //  SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));


    QCamera::CaptureModes captureMode = QCamera::CaptureStillImage;

    if (m_SelectedCamera->isCaptureModeSupported(captureMode))
        m_SelectedCamera->setCaptureMode(captureMode);

    m_SelectedCamera->start();

}


void KPPBarcodeReader::setCamera(int Index)
{
    if(QCameraInfo::availableCameras().count()>=Index){
        QCameraInfo thecamera=QCameraInfo::availableCameras().at(Index);
        setCamera(&thecamera);
    }
}




void KPPBarcodeReader::CaptureTimer()
{

    if(m_captureEnabled==true && m_CameraCapture->isReadyForCapture()){
        m_CameraCapture->capture();
    }
    if(decodeType()!=OneShot)
        timer_getImage->start();
    //if(decode)
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


