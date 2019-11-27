#include "sybersdkupdateplugin.h"
#include "sybersdkupdateconstants.h"
#include "checkupdatethead.h"
#include "updatedialog.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QDir>
#include <QApplication>

namespace SyberSDKUpdate {
namespace Internal {

static QString syberSDKResourcePath()
{
    QString strUserResourcePath = Core::ICore::userResourcePath();
    int nPos = strUserResourcePath.lastIndexOf(QLatin1String("/"));
    return strUserResourcePath.mid(0, nPos);
}


SyberSDKUpdatePlugin::SyberSDKUpdatePlugin()
{
    // Create your members
    checkUpdateThread=new CheckUpdateThead(this);
    updateDialog=new UpdateDialog;
    updateDialog->setModal(true);
    updateDialog->hide();
}

SyberSDKUpdatePlugin::~SyberSDKUpdatePlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool SyberSDKUpdatePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)


    QAction *action = new QAction(tr("版本检测"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    connect(action, &QAction::triggered, this, &SyberSDKUpdatePlugin::updateSyberSDK);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("在线更新"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    connect(checkUpdateThread,SIGNAL(update()),this,SLOT(addChanglog()));
    connect(checkUpdateThread,SIGNAL(NoUpdate()),this,SLOT(showNoUpdateDialog()));
    connect(checkUpdateThread,SIGNAL(writeChanglog()),this,SLOT(showUpdatesDialog()));
    connect(checkUpdateThread,SIGNAL(downloadChanglogFileError(QString)),this,SLOT(showUpdatesDialog()));
    connect(checkUpdateThread,SIGNAL(networkError()),this,SLOT(showNetworkErrorDialog()));
    connect(updateDialog,SIGNAL(execupdate()),this,SLOT(execUpdate()));
    connect(updateDialog,SIGNAL(writeNoTipFlag()),this,SLOT(writeNoTipFlagXmlFile()));
    checkUpdateThread->setRepositoriesXmlpathDir(syberSDKResourcePath());
    //   checkUpdateThread->start();
    checkUpdateThread->run();
    return true;
}

void SyberSDKUpdatePlugin::execUpdate()
{
    updateDialog->hide();

    QProcess *proc=new QProcess;
    connect(proc, &QProcess::errorOccurred,this,&SyberSDKUpdatePlugin::processError);
    QString appPath=QApplication::applicationDirPath();
    QDir appdir(appPath);
    appdir.cdUp();
    appdir.cdUp();
    QString program=appdir.absolutePath()+QUpdate::maintenancetoolName;
    proc->startDetached(program);
    proc->waitForStarted();
    delete proc;

}

void SyberSDKUpdatePlugin::addChanglog()
{
    if(!updateDialog->isHidden())
        updateDialog->hide();

    checkUpdateThread->setDownChangelogFlag(1);
    checkUpdateThread->downloadUpdateinfofile();

}

void SyberSDKUpdatePlugin::writeNoTipFlagXmlFile()
{
    checkUpdateThread->writeNoTipFlagXmlFile(1);
    updateDialog->closeDialog(true);
}

void SyberSDKUpdatePlugin::showUpdatesDialog()
{
    updateDialog->setUpdateTipText();
    updateDialog->parseChangelog(checkUpdateThread->changleLogFileName());

    updateDialog->show();
}

void SyberSDKUpdatePlugin::processError(QProcess::ProcessError error)
{
    qDebug()<<tr("process error")<<error;

}

void SyberSDKUpdatePlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag SyberSDKUpdatePlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void SyberSDKUpdatePlugin::showNoUpdateDialog()
{
    if(checkUpdateThread->enableFlag()==1)
    {
        checkUpdateThread->setEnabelFlag(0);
        if(updateDialog->isHidden())
            updateDialog->hide();

        updateDialog->setNoupdateTiptext();
        //          updateDialog->setModal(true);
        updateDialog->show();
    }
}

void SyberSDKUpdatePlugin::showNetworkErrorDialog()
{

    if(checkUpdateThread->enableFlag()==1)
    {
        checkUpdateThread->setEnabelFlag(0);
        if(updateDialog->isHidden())
            updateDialog->hide();

        updateDialog->setNetworkErrorInfoText();
        //           updateDialog->setModal(true);
        updateDialog->show();
    }
}

void SyberSDKUpdatePlugin::updateSyberSDK()
{
    //    QMessageBox::information(Core::ICore::mainWindow(), tr("Wanning"), tr("No latest version"));
    //return;

    //get path
    // QString strPath = Core::ICore::libexecPath();
    //      QMessageBox::information(Core::ICore::mainWindow(), tr("Wanning"), tr("No latest version %1").arg(strPath));
    //    int nPos = strPath.lastIndexOf(QLatin1String("/"));
    //    QString strUpdateProcessPath = strPath.mid(0, nPos) + QLatin1String("/Update/SdkUpdate");
    //   //start process to check syberos version , if has new version update.
    //    QProcess::startDetached(strUpdateProcessPath, QStringList()<<tr("Manual"), tr(""));
    //this->enableflag=1;

    checkUpdateThread->setEnabelFlag(1);

    checkUpdateThread->updateChecker();
}


} // namespace Internal
} // namespace SyberSDKUpdate
