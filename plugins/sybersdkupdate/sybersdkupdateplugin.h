#ifndef SYBERSDKUPDATE_H
#define SYBERSDKUPDATE_H

#include "checkupdatethead.h"
#include "sybersdkupdate_global.h"
#include "updatedialog.h"

#include <extensionsystem/iplugin.h>

#include <QProcess>

namespace SyberSDKUpdate {
namespace Internal {

class SyberSDKUpdatePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "SyberSDKUpdate.json")

public:
    SyberSDKUpdatePlugin();
    ~SyberSDKUpdatePlugin();
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void updateSyberSDK(void);
    void showUpdatesDialog();
    void showNoUpdateDialog();
    void showNetworkErrorDialog();
    void processError(QProcess::ProcessError error);
    void execUpdate();
    void addChanglog();
    void writeNoTipFlagXmlFile();

private:
    CheckUpdateThead *checkUpdateThread;
    UpdateDialog *updateDialog;
 //   int enableflag;
};

} // namespace Internal
} // namespace SyberSDKUpdate

#endif // SYBERSDKUPDATE_H
