#ifndef KPPBARCODEREADER_H
#define KPPBARCODEREADER_H

#include <kppbarcodereader_global.h>
#include <QtCore/qglobal.h>
#include "QZXing.h"
#include <QObject>
#include <QTimer>
#include "visionprocessing.h"
#include<QGraphicsView>
#include <QGraphicsPixmapItem>
#include "visionthread.h"


using namespace GPIO;

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


    void DisableDecoding();
    void EnableDecoding();
    DecodeType decodeType() const;

    void setDecodeType(const DecodeType &decodeType);





private:
    bool isCapturingImage;
    QGraphicsView *m_viewer;



    VisionThread* m_visionthread;

    bool m_captureEnabled;

    QGraphicsScene* scene_processed;


    QZXing *decoder;
    VisionProcessing *m_visionprocessing;

    DecodeType m_decodeType;

    QGraphicsPixmapItem *m_CapturedPixmap;

    void DoWork();
public slots:
    void CaptureImageReady(const QImage &image);
private slots:



signals:
    void BarcodesFound(QList<QString>);
};

#endif // KPPBARCODEREADER_H
