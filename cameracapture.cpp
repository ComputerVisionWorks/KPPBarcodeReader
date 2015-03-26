#include "cameracapture.h"
#include <QMessageBox>


CameraCapture::CameraCapture(QObject *parent) : QObject(parent)
{

//    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
//      cameraInfo.deviceName()
//    }
    isCapturingImage=false;
    m_ImageCapture=0;
    m_SelectedCamera=0;
    m_cameraInfo=0;
}


void CameraCapture::setCamera(QCameraInfo *cameraInfo)
{
    if(cameraInfo==m_cameraInfo){
        return;
    }
    //m_SelectedCamera->deviceDescription()
    delete m_ImageCapture;
    delete m_SelectedCamera;
    //m_ImageCapture=0;
    //m_SelectedCamera=0;

    m_cameraInfo=cameraInfo;
    m_SelectedCamera = new QCamera(*cameraInfo);


    m_ImageCapture = new QCameraImageCapture(m_SelectedCamera);


    //updateCameraState(m_SelectedCamera->state());

    connect(m_ImageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(m_ImageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));

    connect(m_ImageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this,
            SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));


    QCamera::CaptureModes captureMode = QCamera::CaptureStillImage;

    if (m_SelectedCamera->isCaptureModeSupported(captureMode))
        m_SelectedCamera->setCaptureMode(captureMode);


   // m_SelectedCamera->start();
}

void CameraCapture::setCamera(int Index)
{
    if(QCameraInfo::availableCameras().count()>=Index){
        QCameraInfo thecamera=QCameraInfo::availableCameras().at(Index);
        setCamera(&thecamera);
    }
}

void CameraCapture::startCamera()
{
    if(m_SelectedCamera!=0)
        m_SelectedCamera->start();
}

void CameraCapture::stopCamera()
{
    if(m_SelectedCamera!=0)
        m_SelectedCamera->stop();
}

void CameraCapture::CaptureImage()
{
    if(m_ImageCapture!=0){
     if(m_ImageCapture->isReadyForCapture()){
         isCapturingImage = true;
         m_ImageCapture->capture();
     }
    }

}

void CameraCapture::displayCaptureError(int id, QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
   // QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;
}

void CameraCapture::displayCameraError()
{
    //QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}

void CameraCapture::processCapturedImage(int requestId, const QImage &img)
{

    emit ImageAvaible(img);
}

void CameraCapture::readyForCapture(bool ready)
{

}
QCamera *CameraCapture::SelectedCamera() const
{
    return m_SelectedCamera;
}



CameraCapture::~CameraCapture()
{

    delete m_ImageCapture;
    delete m_SelectedCamera;
}

