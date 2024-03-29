DEFINES += SYBERSDK_LIBRARY

QT += network quick xml gui qml

# SyberSDK files

SOURCES += sybersdkplugin.cpp \
           deviceitem.cpp \
	   developermanager.cpp \
	   objectlistmodel.cpp \
           chooselist.cpp \
           chooselist_forbuild.cpp

HEADERS += sybersdkplugin.h \
           sybersdk_global.h \
           sybersdkconstants.h \
	   deviceitem.h \
	   developermanager.h \
           objectlistmodel.h \
           chooselist.h \
           chooselist_forbuild.h

#INCLUDEPATH += /usr/include/python3.6m

#LIBS += -lpython3.6m


# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "/data/qt-creator-opensource-src-4.10.1"

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "/data/qt-creator-opensource-src-4.10.1"

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = SyberSDK
QTC_LIB_DEPENDS +=     # nothing here at this time

QTC_PLUGIN_DEPENDS +=     coreplugin

QTC_PLUGIN_RECOMMENDS +=     # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

RESOURCES += \
    syber.qrc

