#ifndef VISIONCAPTURE_H
#define VISIONCAPTURE_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QBasicTimer>
#include <QTimerEvent>
#include "kppbarcodereader_global.h"

class KPPBARCODEREADER_EXPORT  VisionCapture : public QObject
{
    Q_OBJECT
public:
    explicit VisionCapture(QObject *parent = 0);
    ~VisionCapture();
    int CaptureInterval() const;
    void setCaptureInterval(int CaptureInterval);

private:
    QBasicTimer m_timer;
    int m_CaptureInterval;
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
