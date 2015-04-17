#ifndef VISIONCAPTURE_H
#define VISIONCAPTURE_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QBasicTimer>
#include <QTimerEvent>

class VisionCapture : public QObject
{
    Q_OBJECT
public:
    explicit VisionCapture(QObject *parent = 0);
    ~VisionCapture();
private:
    QBasicTimer m_timer;
    QScopedPointer<cv::VideoCapture> m_videoCapture;
signals:
    void CaptureStarted();
    void FrameReady(const cv::Mat &);
public slots:
    void StartCapture(int cam = 0);
    void StopCapture();

    // QObject interface
protected:
    void timerEvent(QTimerEvent *);
};



#endif // VISIONCAPTURE_H
