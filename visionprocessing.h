#ifndef VISIONPROCESSING_H
#define VISIONPROCESSING_H

#include <kppbarcodereader_global.h>
#include <QObject>
#include "opencv2/opencv.hpp"
#include <QImage>
#include "QZXing.h"
#include "QGraphicsPixmapItem"

using namespace cv;

class KPPBARCODEREADER_EXPORT VisionProcessing : public QObject
{
    Q_OBJECT
public:
    explicit VisionProcessing(QObject *parent = 0);
    ~VisionProcessing();
    QList<QString> getBarcodeFromImage(Mat original, QZXing *decoder=0, QGraphicsPixmapItem *pixmap=0);
    static QImage cvMat2QImage(cv::Mat mat_img);
    static Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData=false, bool swap=false);
private:
    //cv::Mat m_PrePorcessedImage;

signals:
    //void ImagePreProcessed(QImage img);
    void BarCodeFound(QString);
public slots:
};

#endif // VISIONPROCESSING_H
