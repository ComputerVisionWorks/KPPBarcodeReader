#ifndef KPPBARCODEREADER_H
#define KPPBARCODEREADER_H

#include <QObject>
#include "visionprocessing.h"
#include <QTimer>
#include "QZXing.h"
#include<QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QCameraInfo>
#include <QCameraImageCapture>

Q_DECLARE_METATYPE(QCameraInfo)

class KPPBarcodeReader:public QObject
{
    Q_OBJECT

    Q_ENUMS(DecodeType)
public:
    enum DecodeType {
        OneShot=0,
        OneShotGoodRead=1,
        Continuous=2
    };

    KPPBarcodeReader(QObject *parent=0, QGraphicsView * viewer=0);
    ~KPPBarcodeReader();

    VisionProcessing *getVisionProcessing() const;
    QList<QString> AvaibleCameras();

    int decodeInterval() const;
    void setDecodeInterval(int decodeInterval);
    void DisableDecoding();
    void EnableDecoding();
    DecodeType decodeType() const;
    void setDecodeType(const DecodeType &decodeType);
    void setCamera(QCameraInfo *cameraInfo=0);
    void setCamera(int Index=-1);
    QCamera *SelectedCamera() const;

    bool captureEnabled() const;

    void setCaptureEnabled(bool captureEnabled);

private:
    bool isCapturingImage;
    QGraphicsView *m_viewer;
    QCamera *m_SelectedCamera;
    QCameraInfo *m_cameraInfo;

    bool m_captureEnabled;
    QCameraImageCapture *m_CameraCapture;
    QGraphicsScene* scene_processed;

    QTimer *timer_getImage;
    QZXing *decoder;
    VisionProcessing *m_visionprocessing;
    DecodeType m_decodeType;
    int m_decodeInterval;
    QGraphicsPixmapItem *m_CapturedPixmap;

public slots:

private slots:
    void ImageAvaible(int, QImage);
    void CaptureTimer();

signals:
    void BarcodesFound(QList<QString>);
};

#endif // KPPBARCODEREADER_H
