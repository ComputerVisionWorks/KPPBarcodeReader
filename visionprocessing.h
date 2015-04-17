#ifndef VISIONPROCESSING_H
#define VISIONPROCESSING_H

#include <kppbarcodereader_global.h>
#include <QObject>
#include "opencv2/opencv.hpp"
#include <QImage>
#include "QZXing.h"
#include "QGraphicsPixmapItem"
#ifdef __linux__
#include "GPIO/GPIOManager.h"
#include "GPIO/GPIOConst.h"
using namespace GPIO;
#endif
#include <QBasicTimer>

using namespace cv;



class KPPBARCODEREADER_EXPORT VisionProcessing : public QObject
{
    Q_OBJECT
public:
    explicit VisionProcessing(QObject *parent = 0,QZXing *decoder=0);
    ~VisionProcessing();

    static const QImage cvMat2QImage(cv::Mat mat_img);
    static Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData=false, bool swap=false);
    double thresh() const;
    void setThresh(double thresh);

    double thresh_inner() const;
    void setThresh_inner(double thresh_inner);    
    static void ImageFrameDeleter(void* mat);
private:
    //cv::Mat m_PrePorcessedImage;
    QZXing *m_decoder;
    double m_thresh;
    double m_thresh_inner;
    #ifdef __linux__
    GPIOManager* m_gpiomanager;
    #endif
    int m_LedsPin;

    QBasicTimer m_timer;
    cv::Mat m_ImageFrame;
    bool m_processAll;
    cv::Mat m_frame;
    void ImageFrameQueue(const cv::Mat & frame);

    void RotateWithoutCrop(const double degrees, Mat &src, Mat &dst);

    Point2f rotPoint(const Mat &R, const Point2f &p);
    Mat rotateImage(const Mat &fromI, Mat *toI, const Rect &fromroi, double angle);
    Size rotatedImageBB(const Mat &R, const Rect &bb);
signals:

    void ImageReady(const QImage &);
    void PreprocessedImageReady(const QImage &);
    void CaptureStarted ();
    void BarCodesFound(QList<QString>);
public slots:

    void ProcessImage(Mat original);
    void ProcessFrame(const cv::Mat & frame);
    // QObject interface
protected:


    // QObject interface
protected:
    void timerEvent(QTimerEvent *);
};

Q_DECLARE_METATYPE(cv::Mat)
#endif // VISIONPROCESSING_H
