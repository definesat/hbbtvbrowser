TEMPLATE = app
TARGET = qthbbtvbrowser
QT += network webkitwidgets widgets

cross_compile: DEFINES += EMBEDDED_BUILD=0

SOURCES += \
    qthbbtvbrowser.cpp \
    browserapp.cpp \
    browserwindow.cpp \
    utils.cpp \
    webpage.cpp \
    webview.cpp \
    fpstimer.cpp \
    cookiejar.cpp

HEADERS += \
    browserapp.h \
    browserwindow.h \
    utils.h \
    webinspector.h \
    webpage.h \
    webview.h \
    fpstimer.h \
    cookiejar.h

contains(QT_CONFIG, opengl) {
    QT += opengl
    DEFINES += QT_CONFIGURED_WITH_OPENGL
}



