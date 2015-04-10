INCLUDEPATH += $$PWD
INCLUDEPATH += .

win32
{
INCLUDEPATH +=$$PWD/../ExternalLibs/OpenCV/include
INCLUDEPATH +=$$PWD/../QZXing/source/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ExternalLibs/OpenCV/lib/ -lopencv_core300 -lopencv_imgproc300 -lopencv_highgui300 -lopencv_imgcodecs300 -lopencv_features2d300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ExternalLibs/OpenCV/lib/ -lopencv_core300d -lopencv_imgproc300d -lopencv_highgui300d -lopencv_imgcodecs300d -lopencv_features2d300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QZXing/source/release/ -lQZXing2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QZXing/source/debug/ -lQZXing2

}



unix{

    INCLUDEPATH +=$$PWD/../QZXing/source/

    LIBS += -lQZXing
    LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_features2d -lopencv_videoio

}
