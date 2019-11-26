#include "sdkmanagerplugin.h"
#include "sdkmanagerconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/id.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>
#include <utils/icon.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QDir>
#include <QVBoxLayout>
#include <QApplication>

using namespace Core;
using namespace ExtensionSystem;
using namespace Utils;

namespace sdkmanager {
namespace Internal {


SdkManagerMode::SdkManagerMode()
{
    setDisplayName(tr("套件"));
    setIcon(QIcon(QLatin1String(":/sdkmanager/images/yuanxin-64.png")));
    setPriority(2); //THe lowest priority
    setId("SdkManager");
    setContextHelp("Qt Creator Manual");
    setContext(Core::Context("sdkmanagerMode"));

    m_modeWidget=new QWidget;
    QVBoxLayout *layout=new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    m_modeWidget->setLayout(layout);

    setWidget(m_modeWidget);

    connect(ModeManager::instance(),&ModeManager::currentModeChanged,this,&SdkManagerMode::startSdkManagerInstallProgram);
}

SdkManagerMode::~SdkManagerMode()
{
    delete m_modeWidget;
}

void SdkManagerMode::startProgram()
{
    QString appPath=QApplication::applicationDirPath();
    QDir appdir(appPath);
    appdir.cdUp();
    QString program=appdir.absolutePath()+QString(QLatin1String("/libexec/qtcreator/sdk-manager"));
    process=new QProcess;
    connect(process,SIGNAL(started()),this,SLOT(startFinish()));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));

    process->start(program);
    process->waitForStarted();
}

void SdkManagerMode::initPlugins()
{
}

void SdkManagerMode::startFinish()
{
    disconnect(process,SIGNAL(started()),this,SLOT(startFinish()));
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishProgram(int,QProcess::ExitStatus)));

}

void SdkManagerMode::finishProgram(int exitCode, QProcess::ExitStatus exitStatus)
{
    disconnect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishProgram(int,QProcess::ExitStatus)));
    disconnect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    if((exitCode==0)&&(exitStatus==QProcess::NormalExit))
    {
                delete process;
                process=nullptr;
    }else
    {
                 QMessageBox::warning(nullptr,tr("error"),tr("The exec install is failed!"));
                 this->killProcess();
    }
}

void SdkManagerMode::processError(QProcess::ProcessError)
{
    disconnect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));

   QMessageBox::warning(nullptr,tr("error"),tr("start failed"));
   this->killProcess();
}

void SdkManagerMode::startSdkManagerInstallProgram(Core::Id mode, Core::Id oldMode)
{
    Q_UNUSED(oldMode)

    if(mode != this->id()){
        return;
    }
    this->startProgram();
}

void SdkManagerMode::killProcess()
{
    if(process)
     {
        if((process->state()==QProcess::Running)||(process->state()==QProcess::Starting))
             process->kill();
    }
}


static bool parseConfigFile(QString configFileName)
{
    QFile configFile(configFileName);
    if(configFile.exists())
    {
        if(!configFile.open(QIODevice::ReadOnly|QIODevice::Text))
                    return false;
                QTextStream in(&configFile);
                while(!in.atEnd())
                {
                    QString line=in.readLine();
                    if(!line.isEmpty())
                    {
                        QStringList listData=line.split(QString(QLatin1String("=")));
                        if(listData.at(0).contains(QString(QLatin1String("sdk-install"))))
                        {
                                QFile file(listData.at(1));
                                if(file.exists())
                                    return true;
                        }
                    }
                }
    }
    return false;
}




sdkmanagerPlugin::sdkmanagerPlugin()
{
}

sdkmanagerPlugin::~sdkmanagerPlugin()
{
    delete sdkmanagerMode;
}

bool sdkmanagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    sdkmanagerMode=new SdkManagerMode;

    QAction *action = new QAction(tr("交叉编译套件管理工具"), this);//QAction *action = new QAction(tr("sdkmanager"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("套件"));//menu->menu()->setTitle(tr("SdkManager"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void sdkmanagerPlugin::extensionsInitialized()
{
}

bool sdkmanagerPlugin::delayedInitialize(){

    QSettings setting(QSettings::NativeFormat,QSettings::UserScope,QString(QLatin1String("qtversions")));
    bool parseResult=parseConfigFile(setting.fileName());
    if(!parseResult)
    {
        int ret= QMessageBox::question(0,tr("安装提示"),tr("未检测到交叉编译套件,是否现在安装?"),QMessageBox::Ok|QMessageBox::Cancel);//int ret= QMessageBox::question(0,tr("Install compile environment"),tr("Detects your system Syberos compiler environment is not installed,the name of Syberos build environment for PDK,you should install it?"),QMessageBox::Ok|QMessageBox::Cancel);
        if(ret==QMessageBox::Ok)
        {
                Core::ModeManager::activateMode(sdkmanagerMode->id());
        }
    }
    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag sdkmanagerPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void sdkmanagerPlugin::triggerAction()
{
    sdkmanagerMode->startProgram();
}

} // namespace Internal
} // namespace sdkmanager
