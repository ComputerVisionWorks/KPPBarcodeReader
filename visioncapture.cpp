#include "visioncapture.h"

VisionCapture::VisionCapture(QObject *parent) : QObject(parent)
{
    m_CaptureInterval=100;


    setCaptureInterval(m_CaptureInterval);
}

VisionCapture::~VisionCapture()
{

}
int VisionCapture::CaptureInterval() const
{
    return m_CaptureInterval;
}

void VisionCapture::setCaptureInterval(int CaptureInterval)
{
    m_CaptureInterval = CaptureInterval;

}


void VisionCapture::InitCapture(int cam,bool start)
{

    if (!m_videoCapture)
        m_videoCapture.reset(new cv::VideoCapture(cam));
    if (m_videoCapture->isOpened()) {
        m_videoCapture->set(CV_CAP_PROP_FRAME_WIDTH,640);
        m_videoCapture->set(CV_CAP_PROP_FRAME_HEIGHT,480);
        m_videoCapture->set(CV_CAP_PROP_FPS,15);
     if(start)
        StartCapture();

    }
}

void VisionCapture::StopCapture()
{
    mutex.lock();
    m_timer.stop();
    mutex.unlock();
}

void VisionCapture::StartCapture()
{
    mutex.lock();
    m_timer.start(m_CaptureInterval,this);
    emit CaptureStarted();
    mutex.unlock();
}

void VisionCapture::timerEvent(QTimerEvent *ev)
{
    mutex.lock();
    if (ev->timerId() != m_timer.timerId()) return;
    if(m_timer.isActive()==false) return;

    if(!m_videoCapture)
        return;
    cv::Mat frame;

    if (!m_videoCapture->read(frame)) { // Blocks until a new frame is ready
        m_timer.stop();
        return;
    }
    emit FrameReady(frame);
    mutex.unlock();
}

