#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T11:03:18
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CurveAligner
TEMPLATE = app
DESTDIR = ../Output
MOC_DIR = ../moc
OBJECTS_DIR = ../obj
#RCC_DIR = ../Resources/qrc_src

AQUILADIR = /opt/aquila
QWTDIR  = /usr/local/qwt-6.1.2
include ( $${QWTDIR}/features/qwt.prf )

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $${QWTDIR}/include
INCLUDEPATH += $${AQUILADIR}/include

LIBS += -L$${AQUILADIR}/lib -lAquila

SOURCES += main.cpp\
        curmainwidget.cpp \
    ffflog.cpp \
    curfilechoosewidget.cpp \
    fffcsvparser.cpp \
    interpolate.cpp \
    fffcsvwriter.cpp

HEADERS  += curmainwidget.h \
    ffflog.h \
    curfilechoosewidget.h \
    fffcsvparser.h \
    interpolate.h \
    us_multi_column.h \
    fffcsvwriter.h
