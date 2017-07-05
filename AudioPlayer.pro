#-------------------------------------------------
#
# Project created by QtCreator 2017-07-03T20:28:59
#
#-------------------------------------------------

QT       += core gui charts multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14
#QMAKE_LFLAGS +=

INCLUDEPATH += /opt/local/include/ \
        /usr/local/include/ \
        /usr/include/

DEPENDPATH += /opt/local/lib/ \
              /usr/local/lib/ \
              /usr/include/

TARGET = AudioPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    audioplayer.cpp \
    oscillogramwidget.cpp

HEADERS += \
        mainwindow.h \
    audioplayer.h \
    oscillogramwidget.h

FORMS += \
        mainwindow.ui

win32:CONFIG(release, debug|release) {
# TODO: Build on windows #
} else:win32:CONFIG(debug, debug|release) {
# TODO: Build on windows #
} else:unix {
        LIBS += /usr/local/lib/libavformat.a \
                /usr/local/lib/libavutil.a \
                /usr/local/lib/libavcodec.a \
                /usr/local/lib/libavresample.a \
                -lavformat -lavutil -lavcodec -lavresample \
                -L/usr/local/lib/ -lao \
                -lz -lm -lbz2

}
