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



void CameraCapture::setCamera(int Index)
{

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


}

