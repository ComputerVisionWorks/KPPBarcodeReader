#include "visionprocessing.h"
#include <QDebug>
#include "QPainter"


VisionProcessing::VisionProcessing(QObject *parent) : QObject(parent)
{

}

VisionProcessing::~VisionProcessing()
{

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

QList<QString> VisionProcessing::getBarcodeFromImage(Mat original, QZXing *decoder,QGraphicsPixmapItem* pixmap)
{
     QList<QString> m_list;

    // asd


    Mat roiImagegray;

    cvtColor(original,roiImagegray,CV_BGR2GRAY);

    Mat roiImageThreshed;
    threshold(roiImagegray,roiImageThreshed,100,255,cv::THRESH_BINARY);
    //pixmap->setPixmap(QPixmap::fromImage(cvMat2QImage(roiImageThreshed)));
    std::vector<std::vector<cv::Point> > contours;
     cv::findContours(roiImageThreshed, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

     std::vector<cv::Point> approx;

     for (int i = 0; i < contours.size(); i++)
         {
             // Approximate contour with accuracy proportional
             // to the contour perimeter
             cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

             // Skip small or non-convex objects
             if (std::fabs(cv::contourArea(contours[i])) < 8000 ||std::fabs(cv::contourArea(contours[i])) > 50000 || !cv::isContourConvex(approx))
                 continue;

             if (approx.size() == 3)
             {
                 //setLabel(dst, "TRI", contours[i]);    // Triangles
             }
             else if (approx.size() >= 4 && approx.size() <= 8)
             {
                 Scalar color = Scalar( 255, 0, 0);
                drawContours( original, contours, i, color, 2, 8, std::vector<Vec4i>(),0, Point() );
                //ProcessingImage.adjustROI()
                Rect boundRect=boundingRect(approx);
                //qDebug()<<"X:"<<boundRect.x;
                //qDebug()<<"Y:"<<boundRect.y;
                //if(boundRect.y-5<0 || boundRect.y+boundRect.height+10>original.size().height ) continue;




                Rect boundRect2(CvPoint(boundRect.x,boundRect.y),CvSize(boundRect.width-boundRect.width*0.3,boundRect.height-7));
                Mat roiimg=original(boundRect2);

                Scalar color2 = Scalar( 0, 0, 255);

                rectangle( original, boundRect2.tl(), boundRect2.br(), color2, 2, 8, 0 );

                if(decoder!=0){
                    QImage roiQimg=cvMat2QImage(roiimg);

                    QString tag= decoder->decodeImage(roiQimg,-1, -1, false);
                    if(tag!=""){

                        //cvMat2QImage()
                       CvPoint txtpt(boundRect.tl());

                       if(txtpt.y<20)
                           txtpt.y=boundRect.height+20;
                        else
                           txtpt.y=boundRect.y-20;

                        putText(original, tag.toStdString(),txtpt,
                            FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0,255,0), 2, CV_AA);


                        m_list.append(tag);
                        emit BarCodeFound(tag);




                    }
                }

                 // Number of vertices of polygonal curve
                 int vtc = approx.size();

                 // Get the cosines of all corners
                 std::vector<double> cos;
                 for (int j = 2; j < vtc+1; j++)
                     cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

                 // Sort ascending the cosine values
                 std::sort(cos.begin(), cos.end());

                 // Get the lowest and the highest cosine
                 double mincos = cos.front();
                 double maxcos = cos.back();

                 // Use the degrees obtained above and the number of vertices
                 // to determine the shape of the contour
                 if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3){
                    // setLabel(dst, "RECT", contours[i]);

                 }
                 else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27){
                     //setLabel(dst, "PENTA", contours[i]);
                 }
                 else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45){
                     //setLabel(dst, "HEXA", contours[i]);
                 }
             }
             else
             {
                 // Detect and label circles
                 double area = cv::contourArea(contours[i]);
                 cv::Rect r = cv::boundingRect(contours[i]);
                 int radius = r.width / 2;

                 if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
                     std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2){
                   //  setLabel(dst, "CIR", contours[i]);
                 }
             }
         }

     if(pixmap!=0){
         cvtColor(original,original,CV_BGR2RGB);
         pixmap->setPixmap(QPixmap::fromImage(cvMat2QImage(original)));





     }


//    std::vector<Mat> channels;
//    channels.push_back(roiImagegray);
//    channels.push_back(roiImagegray);
//    channels.push_back(roiImagegray);

//    merge(channels,roiImagegray);



    //emit ImagePreProcessed(cvMat2QImage(roiImageThreshed));


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
