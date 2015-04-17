#include "visionprocessing.h"
#include <QDebug>
#include "QPainter"



using namespace cv;

VisionProcessing::VisionProcessing(QObject *parent, QZXing *decoder) : QObject(parent)
{
    m_thresh=50;
    m_thresh_inner=50;

    m_processAll=false;

#ifdef __linux__
    m_gpiomanager=GPIOManager::getInstance();

    m_LedsPin = GPIOConst::getInstance()->getGpioByKey("P8_13");

    m_gpiomanager->exportPin(m_LedsPin);
    m_gpiomanager->setDirection(m_LedsPin,GPIO::OUTPUT);
#endif

    m_decoder=decoder;


}


VisionProcessing::~VisionProcessing()
{

#ifdef __linux__
    delete m_gpiomanager;
#endif
}



void VisionProcessing::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() != m_timer.timerId()) return;
    ProcessImage(m_frame);
    m_frame.release();
    m_timer.stop();
}


void VisionProcessing::ProcessImage(Mat original)
{
    QList<QString> m_list;


    // try
    //{


    Mat imagegray,image_edges;

    cvtColor(original,imagegray,CV_BGR2GRAY);
    int ratio = 3;

    /// Reduce noise with a kernel 3x3
    // blur( imagegray, imagegray, Size(3,3) );
    int kernel_size = 3;
    Canny( imagegray, image_edges, m_thresh, m_thresh*ratio, kernel_size );



     // threshold(imagegray,imagegray,m_thresh,255,cv::THRESH_BINARY);


    std::vector<std::vector<cv::Point> > image_contours;

    cv::findContours(image_edges, image_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    /*
        dst = Mat::zeros(Imagegray.size().height,Imagegray.size().width,original.type());

        cvtColor(detected_edges,detected_edges,CV_GRAY2BGR);

        dst.copyTo( original, detected_edges);
        */

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
                std::fabs(area)> 200000 ||
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

            Mat labelimage=original(label_boundrect);





            Mat barcode_edges,labelroigray;

            cvtColor(labelimage,labelroigray,CV_BGR2GRAY);

            /// Reduce noise with a kernel 3x3
            //  blur( labelroigray, labelroigray, Size(3,3) );
            int kernel_size = 3;

            dilate(labelroigray, labelroigray,Mat(), Point(-1, -1), 1, 1, 1);
            double thr1=100;
            Canny( labelroigray, barcode_edges, thr1,thr1*2, kernel_size );

            //Mat labelroi;
            //threshold(labelroigray,labelroigray,m_thresh,255,cv::THRESH_BINARY);



            std::vector<std::vector<cv::Point> > in_labelcontours;
            cv::findContours(barcode_edges, in_labelcontours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

            std::vector<cv::Point> barcode_approx;

            for (int i = 0; i < in_labelcontours.size(); i++)
            {
                Scalar color_in_label = Scalar( 0, 255, 0);
                Scalar color_barcode_rect = Scalar( 0, 0, 255);
                //drawContours( labelimage, in_labelcontours, i, color3, 2, 8, std::vector<Vec4i>(),0, Point() );
                // Approximate contour with accuracy proportional
                // to the contour perimeter
                cv::approxPolyDP(cv::Mat(in_labelcontours[i]), barcode_approx, cv::arcLength(cv::Mat(in_labelcontours[i]), true)*0.02, true);

                if(barcode_approx.size()<2) continue;

                Rect barcode_boundrect=boundingRect(barcode_approx);

                //RotatedRect barcodeRect =minAreaRect(in_labelcontours[i]);
                double area=contourArea(in_labelcontours[i]);
                double perimeter = cv::arcLength(in_labelcontours[i],true);
                if(perimeter<20) continue;

                if(barcode_boundrect.height<20) continue;
                if(barcode_boundrect.width<20) continue;
                if(barcode_boundrect.width>300) continue;

                /*                          barcode_boundrect.height<0 ||
                            barcode_boundrect.height>1000 ||
                            barcode_boundrect.width<0 ||
                            barcode_boundrect.width>1000 ||
                            std::fabs(area) <30 ||
                            std::fabs(area) >10000
                            )
                        continue;
*/
                drawContours( labelimage, in_labelcontours, i, color_in_label, 2, 8, std::vector<Vec4i>(),0, Point() );

                //   rectangle( labelimage, barcode_boundrect.tl(), barcode_boundrect.br(), color_barcode_rect, 2, 8, 0 );


                Rect barcode_boundrect_adjusted=Rect(label_boundrect.tl(),Size(barcode_boundrect.size().width*2.5,barcode_boundrect.size().height*2.5));

                barcode_boundrect_adjusted-=barcode_boundrect.tl();





                /*
                    Scalar color2 = Scalar( 0, 0, 255);



                    Rect barcode_boundrect_adjusted=barcode_boundrect;//+cv::Size(barcode_boundrect.width, barcode_boundrect.height);
                    //barcode_boundrect_adjusted.x-=barcode_boundrect.width/2;
                    //barcode_boundrect_adjusted.y-=barcode_boundrect.height/2;
 */


                if(barcode_boundrect_adjusted.x<=0){
                    barcode_boundrect_adjusted.x=1;
                }

                if(barcode_boundrect_adjusted.y<=0){
                    barcode_boundrect_adjusted.y=1;
                }

                if(barcode_boundrect_adjusted.width+barcode_boundrect_adjusted.x>=original.size().width){
                    barcode_boundrect_adjusted.width=original.size().width-barcode_boundrect_adjusted.x;
                }

                if(barcode_boundrect_adjusted.height+barcode_boundrect_adjusted.y>=original.size().height){
                    barcode_boundrect_adjusted.height=original.size().height-barcode_boundrect_adjusted.y;
                }

                //   rectangle( original, barcode_boundrect_adjusted.tl(), barcode_boundrect_adjusted.br(), color2, 2, 8, 0 );



                Mat barcodeimg=original(barcode_boundrect_adjusted);




                if(m_decoder!=0){
                    QImage roiQimg=cvMat2QImage(barcodeimg);

                    QString tag= m_decoder->decodeImage(roiQimg,-1, -1, false);

                    if(tag!=""){
                        rectangle( original, barcode_boundrect_adjusted.tl(), barcode_boundrect_adjusted.br(), color_barcode_rect, 2, 8, 0 );
                        //cvMat2QImage()
                        CvPoint txtpt(label_boundrect.tl());

                        if(txtpt.y<20)
                            txtpt.y=label_boundrect.height+20;
                        else
                            txtpt.y=label_boundrect.y-20;

                        putText(original, tag.toStdString(),txtpt,
                                FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0,255,0), 2, CV_AA);


                        m_list.append(tag);
                        break;
                        //emit BarCodeFound(tag);




                    }

                }


            }

        }
    }

    cvtColor(original,original,CV_BGR2RGB);

    const QImage image=VisionProcessing::cvMat2QImage(original);

    Q_ASSERT(image.constBits() == original.data);
    emit ImageReady(image);
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

void VisionProcessing::ImageFrameQueue(const Mat &frame)
{
    if (!m_frame.empty()) qDebug() << "Converter dropped frame!";
    m_frame = frame;
    if (! m_timer.isActive()) m_timer.start(0, this);
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

