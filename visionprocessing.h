#ifndef VISIONPROCESSING_H
#define VISIONPROCESSING_H

#include <QObject>
#include "opencv2/opencv.hpp"
#include <QImage>
#include "QZXing.h"

using namespace cv;

class VisionProcessing : public QObject
{
    Q_OBJECT
public:
    explicit VisionProcessing(QObject *parent = 0);
    ~VisionProcessing();
    QList<QString> getBarcodeFromImage(QImage original, QZXing *decoder=0);
    static QImage cvMat2QImage(cv::Mat mat_img);
    static Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData=false);
private:
    //cv::Mat m_PrePorcessedImage;

signals:
    //void ImagePreProcessed(QImage img);
    void BarCodeFound(QString);
public slots:
};

#endif // VISIONPROCESSING_H
