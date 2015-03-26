#ifndef KPPBARCODEREADER_H
#define KPPBARCODEREADER_H

#include <QObject>
#include "cameracapture.h"
#include "visionprocessing.h"
#include <QTimer>
#include <QZXing.h>



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

    KPPBarcodeReader(QObject *parent=0);
    ~KPPBarcodeReader();
    CameraCapture *Capture() const;
    //void setCapture(CameraCapture *Capture);
    VisionProcessing *getVisionProcessing() const;
    QList<QString> AvaibleCameras();

    int decodeInterval() const;
    void setDecodeInterval(int decodeInterval);

    DecodeType decodeType() const;
    void setDecodeType(const DecodeType &decodeType);

public slots:
    void StartDecoding();
    void StopDecoding();
private slots:
    void ImageAvaible(QImage);
private:
    CameraCapture* m_Capture;
    QTimer *timer_getImage;
    QZXing *decoder;
    VisionProcessing *m_visionprocessing;
    DecodeType m_decodeType;
    int m_decodeInterval;
signals:
    void BarcodesFound(QList<QString>);
};

#endif // KPPBARCODEREADER_H
