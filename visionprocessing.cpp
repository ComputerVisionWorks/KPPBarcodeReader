#include "visionprocessing.h"
#include <QDebug>
#include "QPainter"
#include "numeric"
#include "math.h"


using namespace cv;

VisionProcessing::VisionProcessing(QObject *parent, QZXing *decoder) : QObject(parent)
{
    m_thresh=180;
    m_thresh_inner=180;

    m_processAll=false;

#ifdef __linux__
    m_gpiomanager=GPIOManager::getInstance();

    m_LedsPin = GPIOConst::getInstance()->getGpioByKey("P8_13");

    m_gpiomanager->exportPin(m_LedsPin);
    m_gpiomanager->setDirection(m_LedsPin,GPIO::OUTPUT);
#endif

    m_decoder=decoder;
    m_DecodeEnabled=true;

    m_decodeType=OneShotGoodRead;
    m_decodeinterval=5000;
}


VisionProcessing::~VisionProcessing()
{

#ifdef __linux__
    delete m_gpiomanager;
#endif
}



void VisionProcessing::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId()==m_decodetimer_interval.timerId()){
        m_DecodeEnabled=true;
        return;
    }
    if (ev->timerId() != m_timer.timerId()) return;

    if(m_DecodeEnabled)
        ProcessImage(m_frame);

    m_frame.release();
    m_timer.stop();
}


void VisionProcessing::ProcessImage(Mat original)
{
    QList<QString> m_list;
    bool gotBarcode=false;

    // try
    //{


    Mat imagegray,image_edges;


    cvtColor(original,imagegray,CV_BGR2GRAY);




    // equalizeHist(imagegray,imagegray);

    int kernel_size = 3;

    //GaussianBlur( imagegray, imagegray, Size(3,3), 1, 1, BORDER_DEFAULT );

    //  Canny( imagegray, image_edges, m_thresh, m_thresh*2, kernel_size );




    //    const QImage image2=VisionProcessing::cvMat2QImage(image_edges);
    //    Q_ASSERT(image2.constBits() == image_edges.data);
    //    emit PreprocessedImageReady(image2);

    //return;

    threshold(imagegray,image_edges,m_thresh,255,cv::THRESH_BINARY);


    std::vector<std::vector<cv::Point> > image_contours;

    cv::findContours(image_edges, image_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


    std::vector<cv::Point> image_contours_approx;

    for (int i = 0; i < image_contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(Mat(image_contours[i]), image_contours_approx, cv::arcLength(cv::Mat(image_contours[i]), true)*0.02, true);
        Rect label_boundrect=boundingRect(image_contours_approx);
        // Skip small or non-convex objects
        double area=contourArea(image_contours[i]);
        if (std::fabs(area) <10000 ||
                std::fabs(area)> 100000 ||
                label_boundrect.width>500 ||
                label_boundrect.tl().x==0 || label_boundrect.tl().y==0 ||
                label_boundrect.br().x>=original.size().width ||
                label_boundrect.br().y>= original.size().height
                )//|| !cv::isContourConvex(approx))
            continue;

        if (image_contours_approx.size() == 3)
        {
            //setLabel(dst, "TRI", contours[i]);    // Triangles
        }
        else if (image_contours_approx.size() >= 4 && image_contours_approx.size() <= 60)
        {
            Scalar color = Scalar( 255, 0, 0);
            drawContours( original, image_contours, i, color, 2, 8, std::vector<Vec4i>(),0, Point() );
            RotatedRect rotrect=minAreaRect(image_contours[i]);


            Moments mu;
            mu = moments( image_contours[i], false );
            Point2f mc;
            mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );
            circle( original, mc, 4, color, -1, 8, 0 );

            double xoffset=0;

            double w=350+xoffset,h=150;
            Rect centerdRect(mc.x-xoffset-w/2,mc.y-h/2,w,h);

            if(centerdRect.x<1 || centerdRect.y<1 || (centerdRect.x+centerdRect.width>original.size().width) || (centerdRect.y+centerdRect.height>original.size().height)) continue;
            rectangle( original,centerdRect,Scalar(0,255,0),1, 8);



            Mat labelimage_origin=original(centerdRect);
            Mat labelimage;

            labelimage_origin.copyTo(labelimage);

            RotateWithoutCrop(rotrect.angle,labelimage,labelimage);

            CvPoint txtpt2(label_boundrect.tl());
            txtpt2.y=label_boundrect.y;

            putText(original, QString::number(rotrect.angle).toStdString() ,txtpt2,
                    FONT_HERSHEY_PLAIN, 1, cvScalar(0,255,0), 2, CV_AA);



            if(m_DecodeEnabled)
                gotBarcode=getBarcodeInRect(original,labelimage,centerdRect);

//                Rect centerdRect2(mc.x+50-w/2,mc.y-h/2,w,h);
//                rectangle( original,centerdRect2,Scalar(255,0,0),1, 8);

//                Mat labelimage_origin2=original(centerdRect2);
//                Mat labelimage2;

//                labelimage_origin2.copyTo(labelimage2);
//                RotateWithoutCrop(rotrect.angle,labelimage2,labelimage2);
//                if(getBarcodeInRect(original,labelimage2,centerdRect2)){
//                    break;
//                }
           // }


        }
    }

    cvtColor(original,original,CV_BGR2RGB);

    const QImage image=VisionProcessing::cvMat2QImage(original);

    Q_ASSERT(image.constBits() == original.data);
    emit ImageReady(image);

    if(gotBarcode){
        if(decodeType()==OneShotGoodRead){
            setDecodeEnabled(false);
        }
        else if(decodeType()==Continuous){
            setDecodeEnabled(false);
            m_decodetimer_interval.start(m_decodeinterval,this);
        }
    }


    /*  }
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
    }
*/
    //emit ImageCaptured(cvMat2QImage(original));
    // return m_list;



}

bool VisionProcessing::getBarcodeInRect(const Mat &original,const Mat &labelimage,const Rect &roi){


    if(m_decoder!=0){
        cv::normalize(labelimage, labelimage, 0, 255, NORM_MINMAX);

        QImage roiQimg=cvMat2QImage(labelimage);

        const QImage image2=VisionProcessing::cvMat2QImage(labelimage);
        Q_ASSERT(image2.constBits() == labelimage.data);
        emit PreprocessedImageReady(image2);

        QString tag= m_decoder->decodeImage(roiQimg,-1, -1, false);

        if(tag!=""){

            //cvMat2QImage()
            CvPoint txtpt(roi.tl());

            txtpt.y=roi.y+roi.height+20;


            putText(original, tag.toStdString(),txtpt,
                    FONT_HERSHEY_PLAIN, 1, cvScalar(0,255,0), 2, CV_AA);


            emit BarCodesFound(tag);

            return true;



        }

    }

    return false;
}

void VisionProcessing::ProcessFrame(const Mat &frame)
{
    if (m_processAll) ProcessImage(frame); else ImageFrameQueue(frame);
}


/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

double VisionProcessing::thresh() const
{
    return m_thresh;
}

void VisionProcessing::setThresh(double thresh)
{
    m_thresh = thresh;
}
double VisionProcessing::thresh_inner() const
{
    return m_thresh_inner;
}

void VisionProcessing::setThresh_inner(double thresh_inner)
{
    m_thresh_inner = thresh_inner;
}

void VisionProcessing::ImageFrameDeleter(void *mat)
{
    delete static_cast<cv::Mat*>(mat);
}
bool VisionProcessing::DecodeEnabled() const
{
    return m_DecodeEnabled;
}

void VisionProcessing::setDecodeEnabled(bool DecodeEnabled)
{
    m_DecodeEnabled = DecodeEnabled;
#ifdef __linux__
    if(m_DecodeEnabled)
        m_gpiomanager->setValue(m_LedsPin,GPIO::HIGH);
    else
        m_gpiomanager->setValue(m_LedsPin,GPIO::LOW);
#endif
}


void VisionProcessing::ImageFrameQueue(const Mat &frame)
{
    if (!m_frame.empty()) qDebug() << "Converter dropped frame!";
    m_frame = frame;

    if (! m_timer.isActive()) m_timer.start(0, this);



}



VisionProcessing::DecodeType VisionProcessing::decodeType() const
{
    return m_decodeType;
}

void VisionProcessing::setDecodeType(const DecodeType &decodeType)
{
    m_decodeType = decodeType;
    m_DecodeEnabled=true;

}
int VisionProcessing::decodeinterval() const
{
    return m_decodeinterval;
}

void VisionProcessing::setDecodeinterval(int decodeinterval)
{
    m_decodeinterval = decodeinterval;

}





const QImage VisionProcessing::cvMat2QImage(cv::Mat mat_img)
{


    switch ( mat_img.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        const QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_RGB32,&ImageFrameDeleter, new cv::Mat(mat_img));

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        const QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_RGB888,&ImageFrameDeleter, new cv::Mat(mat_img));

        return image;
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_Indexed8,&ImageFrameDeleter, new cv::Mat(mat_img));

        image.setColorTable( sColorTable );
        const QImage image2(image);
        return image2;
    }

    default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << mat_img.type();
        break;
    }

    return QImage();
}


cv::Mat VisionProcessing::QImageToCvMat( const QImage &inImage, bool inCloneImageData,bool swap)
{
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_RGB32:
    {
        cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

        return (inCloneImageData ? mat.clone() : mat);
    }

        // 8-bit, 3 channel
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

        QImage   swapped;
        if(swap)
            swapped= inImage.rgbSwapped();
        else
            swapped=inImage;

        return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
    }

        // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}


void VisionProcessing::RotateWithoutCrop(const double degrees,Mat &src,Mat &dst)
{

    cv::Mat frame, frameRotated;

    int diagonal = (int)sqrt(src.cols * src.cols + src.rows * src.rows);
    int newWidth = diagonal;
    int newHeight = diagonal;

    int offsetX = (newWidth - src.cols) / 2;
    int offsetY = (newHeight - src.rows) / 2;
    cv::Mat targetMat(newWidth, newHeight, src.type(), cv::Scalar(0));
    cv::Point2f src_center(targetMat.cols / 2.0f, targetMat.rows / 2.0f);

    src.copyTo(frame);

    frame.copyTo(targetMat.rowRange(offsetY, offsetY +
                                    frame.rows).colRange(offsetX, offsetX + frame.cols));
    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, degrees, 1.0);
    cv::warpAffine(targetMat, frameRotated, rot_mat, targetMat.size());

    cv::Rect bound_Rect(frame.cols, frame.rows, 0, 0);
    int x1 = offsetX;
    int x2 = offsetX + frame.cols;
    int x3 = offsetX;
    int x4 = offsetX + frame.cols;
    int y1 = offsetY;
    int y2 = offsetY;
    int y3 = offsetY + frame.rows;
    int y4 = offsetY + frame.rows;
    cv::Mat co_Ordinate = (cv::Mat_<double>(3, 4) << x1, x2, x3, x4,
                           y1, y2, y3, y4,
                           1,  1,  1,  1 );

    cv::Mat RotCo_Ordinate = rot_mat * co_Ordinate;

    for (int i = 0; i < 4; ++i) {
        if (RotCo_Ordinate.at<double>(0, i) < bound_Rect.x)
            bound_Rect.x = (int)RotCo_Ordinate.at<double>(0, i);
        if (RotCo_Ordinate.at<double>(1, i) < bound_Rect.y)
            bound_Rect.y = RotCo_Ordinate.at<double>(1, i);
    }

    for (int i = 0; i < 4; ++i) {
        if (RotCo_Ordinate.at<double>(0, i) > bound_Rect.width)
            bound_Rect.width = (int)RotCo_Ordinate.at<double>(0, i);
        if (RotCo_Ordinate.at<double>(1, i) > bound_Rect.height)
            bound_Rect.height = RotCo_Ordinate.at<double>(1, i);
    }

    bound_Rect.width = bound_Rect.width - bound_Rect.x;
    bound_Rect.height = bound_Rect.height - bound_Rect.y;

    if (bound_Rect.x < 0)
        bound_Rect.x = 0;
    if (bound_Rect.y < 0)
        bound_Rect.y = 0;
    if (bound_Rect.width > frameRotated.cols)
        bound_Rect.width = frameRotated.cols;
    if (bound_Rect.height > frameRotated.rows)
        bound_Rect.height = frameRotated.rows;

    cv::Mat ROI = frameRotated(bound_Rect);
    ROI.copyTo(dst);
}
