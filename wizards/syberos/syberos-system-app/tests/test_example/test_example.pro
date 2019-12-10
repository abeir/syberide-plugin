QT       += testlib
QT       -= gui

TARGET = tst_example
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += link_pkgconfig

PKGCONFIG += syberos-application syberos-application-cache syberos-qt-system

SOURCES += test_example.cpp

# Installation path
target.path = /data/apps/syberos.unit.case/

INSTALLS += target

