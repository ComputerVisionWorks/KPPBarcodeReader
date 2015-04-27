#include "kppbarcodereader.h"

#ifdef __linux__
#if defined(BBB)
using namespace GPIO;
#endif
#endif

Q_DECLARE_METATYPE(VisionProcessing::DecodeType)

KPPBarcodeReader::KPPBarcodeReader(QObject *parent, QGraphicsView *viewer, QGraphicsView *viewer_pre) :
    QObject(parent)
{



    qRegisterMetaType<cv::Mat>();

    m_captureEnabled=false;

    m_CapturedPixmap=0;
    m_viewer=viewer;
    if(viewer!=0){
        scene_processed = new QGraphicsScene();
        m_CapturedPixmap=scene_processed->addPixmap(QPixmap());
        m_viewer->setScene(scene_processed);
    }


    m_PreprocessedPixmap=0;
    m_viewerPreProcessed=viewer_pre;
    if(m_viewerPreProcessed!=0){
        scene_PreProcessed = new QGraphicsScene();
        m_PreprocessedPixmap=scene_PreProcessed->addPixmap(QPixmap());
        m_viewerPreProcessed->setScene(scene_PreProcessed);
    }


    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);



    m_visioncapture=new VisionCapture();
    m_visionprocessing = new VisionProcessing(0,decoder);

    m_processingthread=new QThread(this);


    m_visionprocessing->moveToThread(m_processingthread);

    connect(m_processingthread, SIGNAL(finished()), m_processingthread, SLOT(deleteLater()));
    connect(m_processingthread, SIGNAL(finished()), m_visionprocessing, SLOT(deleteLater()));

    connect(m_visioncapture,SIGNAL(FrameReady(cv::Mat)),m_visionprocessing,SLOT(ProcessFrame(cv::Mat)));
    connect(m_visionprocessing,SIGNAL(ImageReady(QImage)),this,SLOT(CaptureImageReady(QImage)));
    connect(m_visionprocessing,SIGNAL(PreprocessedImageReady(QImage)),this,SLOT(PreProcessedImageReady(QImage)));

    m_capturethread=new QThread(this);
    connect(m_capturethread, SIGNAL(finished()), m_capturethread, SLOT(deleteLater()));

    connect(m_capturethread, SIGNAL(finished()), m_visioncapture, SLOT(deleteLater()));

    connect(m_capturethread, SIGNAL(started()),this,SLOT(CaptureStarted()));

    m_visioncapture->moveToThread(m_capturethread);

    m_processingthread->start();

    m_capturethread->start();



    QMetaObject::invokeMethod(m_visioncapture, "StartCapture");

}


KPPBarcodeReader::~KPPBarcodeReader()
{
    m_processingthread->Resume();
    m_processingthread->requestInterruption();
    m_processingthread->wait();
    m_capturethread->Resume();
    m_capturethread->requestInterruption();
    m_capturethread->wait();
    delete decoder;


}



void KPPBarcodeReader::ShowCaptureImages(bool value)
{


}
bool KPPBarcodeReader::captureEnabled() const
{
    return m_captureEnabled;
}

void KPPBarcodeReader::setCaptureEnabled(bool captureEnabled)
{
    m_captureEnabled = captureEnabled;
    if(m_captureEnabled){
        if(m_capturethread->isRunning()==false){
         m_capturethread->Resume();
        }
    }
    else{
        if(m_capturethread->isRunning()==true){
            m_capturethread->Pause();
        }
    }
}




void KPPBarcodeReader::CaptureImageReady(const QImage &image)
{

    m_CapturedPixmap->setPixmap(QPixmap::fromImage(image));
    m_viewer->fitInView(m_CapturedPixmap,Qt::KeepAspectRatio);
}

void KPPBarcodeReader::PreProcessedImageReady(const QImage &image)
{
    m_PreprocessedPixmap->setPixmap(QPixmap::fromImage(image));
    m_viewerPreProcessed->fitInView(m_PreprocessedPixmap,Qt::KeepAspectRatio);
}

void KPPBarcodeReader::CaptureStarted()
{
    qDebug() << "capture started";
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


