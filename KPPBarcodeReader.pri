
QT       += widgets multimedia

QT       -= gui

TARGET = KPPBarcodeReader




SOURCES += $$PWD/kppbarcodereader.cpp \
    $$PWD/visionprocessing.cpp

HEADERS += $$PWD/kppbarcodereader.h \
    $$PWD/visionprocessing.h

win32{
INCLUDEPATH += $$PWD

INCLUDEPATH +=$$PWD/../ExternalLibs/OpenCV/include
INCLUDEPATH +=$$PWD/../QZXing/source/


}

unix {    
    target.path = /usr/lib
    INSTALLS += target

}



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ExternalLibs/OpenCV/lib/ -lopencv_core300 -lopencv_imgproc300 -lopencv_highgui300 -lopencv_imgcodecs300 -lopencv_features2d300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ExternalLibs/OpenCV/lib/ -lopencv_core300d -lopencv_imgproc300d -lopencv_highgui300d -lopencv_imgcodecs300d -lopencv_features2d300d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QZXing/source/release/ -lQZXing2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QZXing/source/debug/ -lQZXing2
else:unix: LIBS += -L/usr/lib/ -lQZXing

#include(../../QZXing/source/QZXing.pri)
