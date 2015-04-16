#ifndef VISIONPROCESSING_H
#define VISIONPROCESSING_H

#include <kppbarcodereader_global.h>
#include <QObject>
#include "opencv2/opencv.hpp"
#include <QImage>
#include "QZXing.h"
#include "QGraphicsPixmapItem"
#include "GPIO/GPIOManager.h"
#include "GPIO/GPIOConst.h"

using namespace cv;
using namespace GPIO;

class KPPBARCODEREADER_EXPORT VisionProcessing : public QObject
{
    Q_OBJECT
public:
    explicit VisionProcessing(QObject *parent = 0,QZXing *decoder=0);
    ~VisionProcessing();
    QList<QString> getBarcodeFromImage(Mat original);
    static QImage cvMat2QImage(cv::Mat mat_img);
    static Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData=false, bool swap=false);
    double thresh() const;
    void setThresh(double thresh);

    double thresh_inner() const;
    void setThresh_inner(double thresh_inner);

    void StopCapture();
    void Capture(int frames=5);
    void CloseCamera();
    void setCamera(int Index);
private:
    //cv::Mat m_PrePorcessedImage;
    QZXing *m_decoder;
    double m_thresh;
    double m_thresh_inner;
    GPIOManager* m_gpiomanager;
    int m_LedsPin;
    VideoCapture* cvcamera;
signals:
    void ImageCaptured(const QImage &img);

    void BarCodesFound(QList<QString>);
public slots:
};

#endif // VISIONPROCESSING_H
