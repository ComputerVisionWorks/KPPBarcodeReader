#include "kppbarcodereader.h"


using namespace GPIO;



KPPBarcodeReader::KPPBarcodeReader(QObject *parent, QGraphicsView *viewer) :
    QObject(parent)
{




    m_captureEnabled=false;
    m_CapturedPixmap=0;
    m_viewer=viewer;
    if(viewer!=0){
        scene_processed = new QGraphicsScene();
        m_CapturedPixmap=scene_processed->addPixmap(QPixmap());
        m_viewer->setScene(scene_processed);
    }




    decoder= new QZXing(this);
    decoder->setDecoder( QZXing::DecoderFormat_DATA_MATRIX);

    m_decodeType=OneShotGoodRead;

    m_visionprocessing = new VisionProcessing(0,decoder);
    m_visionthread=new VisionThread(m_visionprocessing);
    m_visionprocessing->moveToThread(m_visionthread);

    connect(m_visionthread, SIGNAL(finished()), m_visionthread, SLOT(deleteLater()));
    connect(m_visionprocessing,SIGNAL(ImageCaptured(QImage)),this,SLOT(CaptureImageReady(QImage)));





    m_visionthread->start();

}


KPPBarcodeReader::~KPPBarcodeReader()
{

    m_visionthread->requestInterruption();
    m_visionthread->wait();
    delete decoder;
    delete m_visionprocessing;

}



void KPPBarcodeReader::EnableDecoding()
{


}

void KPPBarcodeReader::DisableDecoding()
{


}



KPPBarcodeReader::DecodeType KPPBarcodeReader::decodeType() const
{
    return m_decodeType;
}

void KPPBarcodeReader::setDecodeType(const DecodeType &decodeType)
{
    m_decodeType = decodeType;
}

void KPPBarcodeReader::CaptureImageReady(const QImage &image)
{

    m_CapturedPixmap->setPixmap(QPixmap::fromImage(image));
    m_viewer->fitInView(m_CapturedPixmap,Qt::KeepAspectRatio);
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


