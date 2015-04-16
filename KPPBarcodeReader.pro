#-------------------------------------------------
#
# Project created by QtCreator 2015-03-24T09:42:47
#
#-------------------------------------------------

QT       += core gui widgets

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


SOURCES += $$PWD/kppbarcodereader.cpp \
    $$PWD/visionprocessing.cpp \
    threadtrigger.cpp

HEADERS += $$PWD/kppbarcodereader.h \
    $$PWD/visionprocessing.h \
    kppbarcodereader_global.h \
    threadtrigger.h

