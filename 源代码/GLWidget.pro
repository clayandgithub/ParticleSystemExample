#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T19:25:52
#
#-------------------------------------------------

QT       += core gui\
opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLWidget
TEMPLATE = app


SOURCES += main.cpp\
        glwidget.cpp \
    particle.cpp \
    centerobj.cpp

HEADERS  += glwidget.h \
    particle.h \
    centerobj.h

FORMS    += glwidget.ui
LIBS     += -lglut32
