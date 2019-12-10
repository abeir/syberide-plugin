#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T16:49:31
#
#-------------------------------------------------

QT       -= gui

TARGET = %ProjectName%

TARGET = $$qtLibraryTarget($$TARGET)

TEMPLATE = lib

DEFINES += %ProjectName:h%PLUGIN_LIBRARY

SOURCES += %ProjectName:l%_plugin.%CppSourceSuffix%

HEADERS += %ProjectName:l%_plugin.%CppHeaderSuffix% \
           include/syberBackupInterface.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
