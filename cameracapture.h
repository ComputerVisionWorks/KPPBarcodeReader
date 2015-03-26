#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QList>

Q_DECLARE_METATYPE(QCameraInfo)

class CameraCapture : public QObject
{
    Q_OBJECT
public:
    explicit CameraCapture(QObject *parent = 0);
    ~CameraCapture();
    void setCamera(QCameraInfo *cameraInfo=0);
    void setCamera(int Index=0);

    QCamera *SelectedCamera() const;

public slots:
    void startCamera();
    void stopCamera();


    void CaptureImage();
    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

    void displayCameraError();


    void processCapturedImage(int requestId, const QImage &img);

    void readyForCapture(bool ready);



private :
    //QList<QCameraInfo*> m_Cameras;
    bool isCapturingImage;
    QCamera *m_SelectedCamera;
    QCameraInfo *m_cameraInfo;
    QCameraImageCapture *m_ImageCapture;

signals:
    void ImageAvaible(const QImage &img);

public slots:
};

#endif // CAMERACAPTURE_H
