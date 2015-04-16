#include "visionprocessing.h"
#include <QDebug>
#include "QPainter"




VisionProcessing::VisionProcessing(QObject *parent, QZXing *decoder) : QObject(parent)
{
    m_thresh=180;
    m_thresh_inner=180;
    cvcamera=0;
    cvcamera= new VideoCapture(0);

    m_gpiomanager=GPIOManager::getInstance();

    m_LedsPin = GPIOConst::getInstance()->getGpioByKey("P8_13");

    m_gpiomanager->exportPin(m_LedsPin);
    m_gpiomanager->setDirection(m_LedsPin,GPIO::OUTPUT);
    m_decoder=decoder;
}

VisionProcessing::~VisionProcessing()
{
    delete cvcamera;
    delete m_gpiomanager;
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

QList<QString> VisionProcessing::getBarcodeFromImage(Mat original)
{
     QList<QString> m_list;

    // asd


    Mat roiImagegray;

    cvtColor(original,roiImagegray,CV_BGR2GRAY);

    Mat roiImageThreshed;
    threshold(roiImagegray,roiImageThreshed,m_thresh,255,cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point> > contours;
     cv::findContours(roiImageThreshed, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

     std::vector<cv::Point> approx;

     for (int i = 0; i < contours.size(); i++)
         {
             // Approximate contour with accuracy proportional
             // to the contour perimeter
             cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
             Rect boundRect=boundingRect(approx);
             // Skip small or non-convex objects
             double area=cv::contourArea(contours[i]);
             if (std::fabs(area) <20000 ||
                 std::fabs(area)> 100000 ||
                 boundRect.tl().x==0 || boundRect.tl().y==0 ||
                 boundRect.br().x>=original.size().width ||
                 boundRect.br().y>= original.size().height
                 )//|| !cv::isContourConvex(approx))
                 continue;

             if (approx.size() == 3)
             {
                 //setLabel(dst, "TRI", contours[i]);    // Triangles
             }
             else if (approx.size() >= 4 && approx.size() <= 60)
             {
                 Scalar color = Scalar( 255, 0, 0);
                drawContours( original, contours, i, color, 2, 8, std::vector<Vec4i>(),0, Point() );
                //ProcessingImage.adjustROI()

                //qDebug()<<"X:"<<boundRect.x;
                //qDebug()<<"Y:"<<boundRect.y;



                //Rect boundRect2(CvPoint(boundRect.x,boundRect.y),CvSize(50,50));

                Mat labelroi=original(boundRect);
                Mat labelroithresh;
                cvtColor(labelroi,labelroithresh,CV_BGR2GRAY);


                threshold(labelroithresh,labelroithresh,m_thresh_inner,255,cv::THRESH_BINARY_INV);

             /*   Mat temp;
                cvtColor(labelroithresh,temp,CV_GRAY2BGR);
                temp.copyTo(labelroi);*/



                std::vector<std::vector<cv::Point> > in_labelcontours;
                cv::findContours(labelroithresh, in_labelcontours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

                 std::vector<cv::Point> barcode_approx;

                for (int i = 0; i < in_labelcontours.size(); i++)
                    {
                        Scalar color3 = Scalar( 0, 255, 0);

                        // Approximate contour with accuracy proportional
                        // to the contour perimeter
                        cv::approxPolyDP(cv::Mat(in_labelcontours[i]), barcode_approx, cv::arcLength(cv::Mat(in_labelcontours[i]), true)*0.02, true);

                        if(barcode_approx.size()<3) continue;

                        Rect boundRect_label=boundingRect(barcode_approx);


                        // Skip small or non-convex objects
                        int gap=50;
                        double area=cv::contourArea(in_labelcontours[i]);
                        if (boundRect_label.width<100 ||
                            boundRect_label.width>250||
                            boundRect_label.height<50 ||
                            boundRect_label.height>160 ||
                            std::fabs(area) <1000 ||
                            std::fabs(area) >80000
                                )
                            continue;

                        drawContours( labelroi, in_labelcontours, i, color3, 2, 8, std::vector<Vec4i>(),0, Point() );

                        Scalar color2 = Scalar( 0, 0, 255);


                        Rect barcode_rect(Point(boundRect.tl().x+boundRect_label.tl().x-gap,boundRect.tl().y+boundRect_label.tl().y-gap), Size(boundRect_label.width+2.3*gap,boundRect_label.height+2*gap));

                        double x,y,w,h;
                        x=barcode_rect.tl().x;
                        y=barcode_rect.tl().y;
                        w=barcode_rect.br().x;
                        h=barcode_rect.br().y;

                        rectangle( original, barcode_rect.tl(), barcode_rect.br(), color2, 2, 8, 0 );

                        Mat barcodeimg=original(barcode_rect);
                       // cv::imwrite("e:/temp.jpeg",barcodeimg);


                        if(m_decoder!=0){
                            QImage roiQimg=cvMat2QImage(barcodeimg);

                            QString tag= m_decoder->decodeImage(roiQimg,-1, -1, false);
                            if(tag!=""){

                                //cvMat2QImage()
                               CvPoint txtpt(barcode_rect.tl());

                               if(txtpt.y<20)
                                   txtpt.y=barcode_rect.height+20;
                                else
                                   txtpt.y=barcode_rect.y-20;

                                putText(original, tag.toStdString(),txtpt,
                                    FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0,255,0), 2, CV_AA);


                                m_list.append(tag);
                                //emit BarCodeFound(tag);




                            }
                        }

                }

             }
         }

    cvtColor(original,original,CV_BGR2RGB);


     emit ImageCaptured(cvMat2QImage(original));
     return m_list;



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





QImage VisionProcessing::cvMat2QImage(cv::Mat mat_img)
{


    switch ( mat_img.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_RGB32);

               return image;
            }

            // 8-bit, 3 channel
            case CV_8UC3:
            {
               QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_RGB888 );

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

               QImage image( mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_Indexed8 );

               image.setColorTable( sColorTable );

               return image;
            }

            default:
               qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << mat_img.type();
               break;
         }

         return QImage();
}




void VisionProcessing::setCamera(int Index)
{

    //if(cvcamera->isOpened()){
    //     cvcamera->release();
    //  }

    cvcamera->open(Index);


    if(cvcamera->isOpened()){
        cvcamera->set(CV_CAP_PROP_FRAME_WIDTH,640);
        cvcamera->set(CV_CAP_PROP_FRAME_HEIGHT,480);



        cvcamera->set(CV_CAP_PROP_FPS,15);
    }

}

void VisionProcessing::CloseCamera()
{
    StopCapture();
    cvcamera->release();
}

void VisionProcessing::StopCapture()
{

}




void VisionProcessing::Capture(int frames)
{
    QList<QString> barcodes;
    try
    {
        if(!cvcamera->isOpened()) return;

        m_gpiomanager->setValue(m_LedsPin,GPIO::HIGH);
        Mat frame;
        int i;
        for(i=0;i<frames;i++){
           bool ret= cvcamera->read(frame); // get a new frame from camera
           if(ret==false){
               cvcamera->release();
               return;
           }
        }
        barcodes=getBarcodeFromImage(frame);
        if(barcodes.count()>0){
           // emit BarcodesFound(barcodes);
            m_gpiomanager->setValue(m_LedsPin,GPIO::LOW);
        }
      //  m_viewer->fitInView(m_CapturedPixmap->boundingRect() ,Qt::KeepAspectRatio);


    }
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;

    }

   // m_gpiomanager->setValue(m_LedsPin,GPIO::LOW);
}

