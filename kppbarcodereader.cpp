#include "kppbarcodereader.h"






KPPBarcodeReader::KPPBarcodeReader(QObject *parent, QGraphicsView *viewer) :
    QObject(parent)
{

    timer_getImage= new QTimer(this);
    timer_getImage->setSingleShot(true);
    timer_getImage->setInterval(250);
    connect(timer_getImage, SIGNAL(timeout()),this, SLOT(Capture()));
    //timer_getImage->start();
    m_visionprocessing = new VisionProcessing(this);

    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);
    m_decodeInterval=50;
    m_decodeType=Continuous;

    cvcamera=0;
    m_captureEnabled=false;
    m_CapturedPixmap=0;
    m_viewer=viewer;
    if(viewer!=0){
        scene_processed = new QGraphicsScene();
        m_CapturedPixmap=scene_processed->addPixmap(QPixmap());
        m_viewer->setScene(scene_processed);
    }

    //    VideoCapture cap(0); // open the default camera
    //        if(cap.isOpened()){  // check if we succeeded
    //            Mat frame;
    //             cap >> frame; // get a new frame from camera
    //        }



}


KPPBarcodeReader::~KPPBarcodeReader()
{
    timer_getImage->stop();
    delete cvcamera;
    delete decoder;
    delete m_visionprocessing;
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



void KPPBarcodeReader::setCamera(int Index)
{

    delete cvcamera;

    cvcamera= new VideoCapture(Index);

    if(cvcamera->isOpened()){
        cvcamera->set(CV_CAP_PROP_FRAME_WIDTH,800);
        cvcamera->set(CV_CAP_PROP_FRAME_HEIGHT,600);
        cvcamera->set(CV_CAP_PROP_FPS,15);
    }

}




void KPPBarcodeReader::Capture()
{
    try
    {
        if(!cvcamera->isOpened()) return;

        Mat frame;
        cvcamera->read(frame); // get a new frame from camera
        m_visionprocessing->getBarcodeFromImage(frame,decoder,m_CapturedPixmap);
        //   m_CapturedPixmap->setPixmap(QPixmap::fromImage(m_visionprocessing->cvMat2QImage(frame)));
        m_viewer->fitInView(m_CapturedPixmap->boundingRect() ,Qt::KeepAspectRatio);

        if(decodeType()!=OneShot)
            timer_getImage->start();
    }
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
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

    //for(int i=0;i<QCameraInfo::availableCameras().count();i++){
    //      cameras.append(QCameraInfo::availableCameras().at(i).description());
    //  }

    return cameras;

}


