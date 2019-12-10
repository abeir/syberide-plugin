#ifndef SYBERBACKUPINTERFACE_H
#define SYBERBACKUPINTERFACE_H

#include <QtPlugin>
#include <QDataStream>

typedef void(*SetProgressFunc)(int);

class SyberBackupRestoreInterface{

public:
    SyberBackupRestoreInterface(){}
    virtual ~SyberBackupRestoreInterface(){}
    virtual bool onBackup(QDataStream* pStream) = 0;
    virtual bool onRestore(QDataStream* pStream) = 0;
    virtual void setProgressCBFunc(SetProgressFunc progressCBFunc) = 0;
    virtual void setProgress(int progress) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SyberBackupRestoreInterface,"SyberBackup.Interface/1.0")
QT_END_NAMESPACE

#endif // SYBERBACKUPINTERFACE_H
