#include "visioncapture.h"

VisionCapture::VisionCapture(QObject *parent) : QObject(parent)
{

}

VisionCapture::~VisionCapture()
{

}

void VisionCapture::StartCapture(int cam)
{

    if (!m_videoCapture)
        m_videoCapture.reset(new cv::VideoCapture(cam));
    if (m_videoCapture->isOpened()) {
        m_videoCapture->set(CV_CAP_PROP_FRAME_WIDTH,640);
        m_videoCapture->set(CV_CAP_PROP_FRAME_HEIGHT,480);
        m_videoCapture->set(CV_CAP_PROP_FPS,15);

        m_timer.start(0, this);
        emit CaptureStarted();
    }
}

void VisionCapture::StopCapture()
{
    m_timer.stop();
}

void VisionCapture::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() != m_timer.timerId()) return;
    cv::Mat frame;
    if (!m_videoCapture->read(frame)) { // Blocks until a new frame is ready
        m_timer.stop();
        return;
    }
    emit FrameReady(frame);
}

