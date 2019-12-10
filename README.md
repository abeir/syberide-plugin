# syberide-plugin

## 准备工作

### 需要安装的lib

* libpam0g-dev


## 插件

假设 qt creator 目录为 `/data/qt-creator-opensource-src-4.10.1`

### wizards
`wizards/syberos` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1/share/qtcreator/templates/wizards` 中
`translations/qtcreator_zh_CN.ts` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1/share/qtcreator/translations` 中

### sdkversion
`sdkversion` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1` 中
`Licenses` 拷贝至目录 `/data` 中
`components.xml` 拷贝至目录 `/data` 中

### sybersdk
`syberos` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1/share/qtcreator` 中

### sdk-manager
`tools/sdk-manager` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1/src/tools` 中
修改 `/data/qt-creator-opensource-src-4.10.1/src/tools/tools.pro`
第5行 SUBDIRS 中添加 sdk-manager 

### syberdevice
`remotelinux/syberdevice` 拷贝至目录 `/data/qt-creator-opensource-src-4.10.1/src/plugins/remotelinux` 中
修改 `/data/qt-creator-opensource-src-4.10.1/src/plugins/remotelinux/remotelinux.pro` 
第4行 添加
```
INCLUDEPATH += $$PWD/syberdevice
INCLUDEPATH += $$PWD
```
末尾添加
```
include(syberdevice/syberdevice.pri)
```