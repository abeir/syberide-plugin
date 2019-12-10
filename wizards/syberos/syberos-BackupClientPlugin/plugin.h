#ifndef %ProjectName:h%_PLUGIN_H
#define %ProjectName:h%_PLUGIN_H

#include "include/syberBackupInterface.h"

#include <qglobal.h>
#include <QObject>

#if defined(%ProjectName:h%PLUGIN_LIBRARY)
#  define %ProjectName:h%PLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define %ProjectName:h%PLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif

class %ProjectName:h%PLUGINSHARED_EXPORT %ProjectName:s%Plugin : public QObject, public SyberBackupRestoreInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "SyberBackup.Interface/1.0")
#endif
    Q_INTERFACES(SyberBackupRestoreInterface)
protected:
    SetProgressFunc p_progressCBFunc;

public:
    %ProjectName:s%Plugin();
    bool onBackup(QDataStream* pStream);
    bool onRestore(QDataStream* pStream);
    void setProgressCBFunc(SetProgressFunc progressCBFunc);
    void setProgress(int progress);
};

#endif // %ProjectName:h%_PLUGIN_H
