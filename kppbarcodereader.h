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
#include "kpptcpserver.h"
#include "kpptcpclientthread.h"

#ifdef __linux__
#if defined(BBB)
using namespace GPIO;
#endif
#endif

class KPPBARCODEREADER_EXPORT KPPBarcodeReader:public QObject
{
    Q_OBJECT



public:


    KPPBarcodeReader(QObject *parent=0, QGraphicsView * viewer=0,QGraphicsView * viewer_pre=0);
    ~KPPBarcodeReader();

    VisionProcessing *getVisionProcessing() const;
    QList<QString> AvaibleCameras();


    void DisableDecoding();
    void ShowCaptureImages(bool value=true);






    bool captureEnabled() const;
    void setCaptureEnabled(bool captureEnabled);

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







public slots:
    void CaptureImageReady(const QImage &image);
    void PreProcessedImageReady(const QImage &image);

private slots:
    void CaptureStarted();
    void NewClientConnected(KPPQtCommon::KPPTCPClientThread* ClientThread );


signals:

};

#endif // KPPBARCODEREADER_H
