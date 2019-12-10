include(../syberos.pri)
QT += gui qml quick widgets

TEMPLATE = app

TARGET = %APPID%

CONFIG += link_pkgconfig

RESOURCES += res.qrc

PKGCONFIG += syberos-application syberos-application-cache

QML_FILES = qml/*.qml

OTHER_FILES += $$QML_FILES *.qm \
    qml/logic.js


# The .cpp file which was generated for your project.
SOURCES += src/main.cpp \
           src/%APPID%_Workspace.cpp

HEADERS += src/%APPID%_Workspace.h

# Installation path
target.path = $$INSTALL_DIR/bin

qm.files = *.qm
qm.path = $$INSTALL_DIR/qm

res.files = res
res.path = $$INSTALL_DIR/

INSTALLS += target qm res

