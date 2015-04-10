#ifndef KPPBARCODEREADER_H
#define KPPBARCODEREADER_H

#include <kppbarcodereader_global.h>

#include <QObject>
#include <QTimer>
#include "visionprocessing.h"
#include<QGraphicsView>
#include <QGraphicsPixmapItem>



class KPPBARCODEREADER_EXPORT KPPBarcodeReader:public QObject
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

    void setCamera(int Index=-1);


    bool captureEnabled() const;

    void setCaptureEnabled(bool captureEnabled);

private:
    bool isCapturingImage;
    QGraphicsView *m_viewer;


    VideoCapture* cvcamera;

    bool m_captureEnabled;

    QGraphicsScene* scene_processed;

    QTimer *timer_getImage;
    QZXing *decoder;
    VisionProcessing *m_visionprocessing;
    DecodeType m_decodeType;
    int m_decodeInterval;
    QGraphicsPixmapItem *m_CapturedPixmap;

public slots:
    void Capture();
private slots:



signals:
    void BarcodesFound(QList<QString>);
};

#endif // KPPBARCODEREADER_H
