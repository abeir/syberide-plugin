#-------------------------------------------------
#
# Project created by QtCreator 2016-04-11T11:22:00
#
#-------------------------------------------------
include(../../qtcreatortool.pri)
include(qtsingleapplication/qtsingleapplication.pri)
include(common/common.pri)
QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sdk-manager
TEMPLATE = app


SOURCES += main.cpp\
    pdkinstallwizard.cpp \
    installmodeselectpage.cpp \
    installbasepage.cpp \
    installoptionspage.cpp \
    selectsdkinstallpathpage.cpp \
    selecttargetinstallpage.cpp \
    installfinishpage.cpp \
    performinstallpage.cpp \
    localinstallselecttargetpage.cpp \
    viewinstalledinfopage.cpp \
    kdrunoncechecker.cpp \
    kdlockfile_p.cpp \
    kdlockfile.cpp

HEADERS  += \
    pdkinstallwizard.h \
    installmodeselectpage.h \
    installbasepage.h \
    installoptionspage.h \
    selectsdkinstallpathpage.h \
    selecttargetinstallpage.h \
    installfinishpage.h \
    performinstallpage.h \
    constField.h \
    localinstallselecttargetpage.h \
    viewinstalledinfopage.h \
    kdrunoncechecker.h \
    kdlockfile_p.h \
    kdlockfile.h \
    kdtoolsglobal.h

FORMS    += widget.ui

CONFIG += c++11

RESOURCES += \
    Resource.qrc

TRANSLATIONS +=sdk-manager.ts

LIBS += -lpam
