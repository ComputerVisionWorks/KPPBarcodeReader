#-------------------------------------------------
#
# Project created by QtCreator 2015-03-24T09:42:47
#
#-------------------------------------------------

QT       += core gui widgets concurrent

TARGET = KPPBarcodeReader
TEMPLATE = lib
CONFIG += plugin
DEFINES = KPPBARCODEREADER_LIBRARY


MOC_DIR = mocs
OBJECTS_DIR = objs

unix {
    target.path = /usr/lib
    INSTALLS += target

}

include(KPPBarcodeReader.pri)
include(../KPPQtCommonLib/KPPQtCommonLib.pri)


SOURCES += $$PWD/kppbarcodereader.cpp \
    $$PWD/visionprocessing.cpp \
    visioncapture.cpp

HEADERS += $$PWD/kppbarcodereader.h \
    $$PWD/visionprocessing.h \
    kppbarcodereader_global.h \
    visioncapture.h

