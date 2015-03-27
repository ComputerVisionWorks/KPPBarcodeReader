#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QList>



class CameraCapture : public QObject
{
    Q_OBJECT
public:
    explicit CameraCapture(QObject *parent = 0);
    ~CameraCapture();




public slots:


    void CaptureImage();
    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

    void displayCameraError();


    void processCapturedImage(int requestId, const QImage &img);

    void readyForCapture(bool ready);



private :



signals:
    void ImageAvaible(const QImage &img);

private slots:

};

#endif // CAMERACAPTURE_H
