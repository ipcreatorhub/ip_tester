#-------------------------------------------------
#
# Project created by QtCreator 2020-01-20T16:07:17
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IP_Tester
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    prbsgenerator10g.cpp \
    tcp_server.cpp \
    tcp_client.cpp \
    console.cpp \
    system_log.cpp \
    serverthread.cpp \
    prbsgenerator.cpp \
    tcpstreamchecker.cpp \
    transferparams.cpp \
    udp_server.cpp

HEADERS += \
        mainwindow.h \
    prbsgenerator10g.h \
    tcp_server.h \
    tcp_client.h \
    console.h \
    system_log.h \
    serverthread.h \
    prbsgenerator.h \
    tcpstreamchecker.h \
    transferparams.h \
    udp_server.h

FORMS += \
        mainwindow.ui \
        transferparams.ui


LIBS += -lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressource.qrc

unix|win32: LIBS += -L$$PWD/npcap-sdk-1.05/Lib/x64/ -lwpcap

INCLUDEPATH += $$PWD/npcap-sdk-1.05/Lib/x64
INCLUDEPATH += $$PWD/npcap-sdk-1.05/Include
DEPENDPATH += $$PWD/npcap-sdk-1.05/Lib/x64
