#ifndef KPPBARCODEREADER_H
#define KPPBARCODEREADER_H

#include <kppbarcodereader_global.h>
#include <QtCore/qglobal.h>
#include "QZXing.h"
#include <QObject>
#include <QTimer>
#include "visionprocessing.h"
#include "visioncapture.h"
#include<QGraphicsView>
#include <QGraphicsPixmapItem>


#ifdef __linux__
using namespace GPIO;
#endif

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

    KPPBarcodeReader(QObject *parent=0, QGraphicsView * viewer=0,QGraphicsView * viewer_pre=0);
    ~KPPBarcodeReader();

    VisionProcessing *getVisionProcessing() const;
    QList<QString> AvaibleCameras();


    void DisableDecoding();
    void ShowCaptureImages(bool value=true);
    DecodeType decodeType() const;

    void setDecodeType(const DecodeType &decodeType);





private:
    bool isCapturingImage;
    QGraphicsView *m_viewer;
    QGraphicsScene* scene_processed;
    QGraphicsPixmapItem *m_CapturedPixmap;

    QGraphicsView *m_viewerPreProcessed;
    QGraphicsScene* scene_PreProcessed;
    QGraphicsPixmapItem *m_PreprocessedPixmap;


   // VisionThread* m_visionthread;

    bool m_captureEnabled;



    QZXing *decoder;
    VisionProcessing *m_visionprocessing;
    VisionCapture* m_visioncapture;
    QThread * m_processingthread;
    QThread * m_capturethread;

    DecodeType m_decodeType;




public slots:
    void CaptureImageReady(const QImage &image);
    void PreProcessedImageReady(const QImage &image);

private slots:
    void CaptureStarted();


signals:
    void BarcodesFound(QList<QString>);
};

#endif // KPPBARCODEREADER_H
