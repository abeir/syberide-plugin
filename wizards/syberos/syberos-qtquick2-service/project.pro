#-------------------------------------------------
#
# Project generated by QtCreator for SyberOS.
#
#-------------------------------------------------

include(syberos.pri)

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = %SERVICEID% %APPID% tests

meta.files = META-INF
meta.path = $$INSTALL_DIR/

syber.files = sopconfig.xml
syber.path = $$INSTALL_DIR/

INSTALLS += meta syber
