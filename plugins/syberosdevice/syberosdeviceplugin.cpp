#include "syberosdeviceplugin.h"
#include "syberosdeviceconstants.h"

#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>

#include <utils/fileutils.h>
#include <utils/hostosinfo.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

using namespace Utils;

namespace SyberOSDevice {
namespace Internal {


SyberOSDeviceMode::SyberOSDeviceMode()
{
    setDisplayName(tr("设备"));
    QIcon qtLogo;
    qtLogo.addFile(QLatin1String(SyberOSDevice::Constants::SYBERDEVICE_32_ICON));
    qtLogo.addFile(QLatin1String(SyberOSDevice::Constants::SYBERDEVICE_64_ICON));
    qtLogo.addFile(QLatin1String(SyberOSDevice::Constants::SYBERDEVICE_128_ICON));
    setIcon(qtLogo);
    setPriority(1); //THe lowest priority`
    setId("SyberOSDevice");
    setContextHelp("Qt Creator SyberOSDevice");
    setContext(Core::Context("Device.SyberOSDeviceMode"));
    createLayout();
}

SyberOSDeviceMode::~SyberOSDeviceMode()
{
    if(m_modeWidget != nullptr)
    {
        delete m_modeWidget;
        m_modeWidget = nullptr;
    }
}

#if QT_VERSION >= 0x050300
void SyberOSDeviceMode::sceneGraphError(QQuickWindow::SceneGraphError, const QString &message)
{
    Q_UNUSED(message)
//    QMessageBox *messageBox =
//        new QMessageBox(QMessageBox::Warning,
//                        tr("SyberSDK Mode Load Error"), message,
//                        QMessageBox::Close, m_modeWidget);
//    messageBox->setModal(false);
//    messageBox->setAttribute(Qt::WA_DeleteOnClose);
//    messageBox->show();
}
#endif // Qt 5.3


void SyberOSDeviceMode::initPlugins()
{
    createLayout();
    //addTab(ProjectExplorerPlugin::instance()->deviceSettingsPage()->widget(), tr("设备管理"));
}

QString SyberOSDeviceMode::platform() const
{
    switch (HostOsInfo::hostOs()) {
    case OsTypeWindows: return QLatin1String("windows");
    case OsTypeMac: return QLatin1String("mac");
    case OsTypeLinux: return QLatin1String("linux");
    case OsTypeOtherUnix: return QLatin1String("unix");
    default: return QLatin1String("other");
    }
}

void SyberOSDeviceMode::createLayout()
{
//    m_mainLayout = new QVBoxLayout;
//    m_mainLayout->setContentsMargins(10, 10, 10, 10);
//    m_mainLayout->setSpacing(10);

    m_modeWidget = new MonitorWidget;
    m_modeWidget->init();
//    m_modeWidget->addTab(ProjectExplorerPlugin::instance()->kitOptionPage()->widget(), tr("构建套件(Kit)"));
//    m_modeWidget->addTab(ProjectExplorerPlugin::instance()->projectExplorerSettingsPage()->widget(), tr("构建设置"));
//    m_modeWidget->addTab(ValgrindPlugin::valgrindOpitonsPage()->widget(), tr("分析器"));

    //connect(ProjectExplorerPlugin::instance()->deviceSettingsPage(), SIGNAL(sigFinish()), this, SLOT(slotSaveSettings()));
//    connect(ProjectExplorerPlugin::instance()->kitOptionPage(), SIGNAL(sigFinish()), this, SLOT(slotSaveSettings()));
//    connect(ProjectExplorerPlugin::instance()->projectExplorerSettingsPage(), SIGNAL(sigFinish()), this, SLOT(slotSaveSettings()));
//    connect(ValgrindPlugin::valgrindOpitonsPage(), SIGNAL(sigFinish()), this, SLOT(slotSaveSettings()));

    //insertTab(0, ProjectExplorerPlugin::instance()->deviceSettingsPage()->widget(), tr("设备管理"));
//    insertTab(1 ,ProjectExplorerPlugin::instance()->kitOptionPage()->widget(), tr("构建套件(Kit)"));
//    insertTab(2, ProjectExplorerPlugin::instance()->projectExplorerSettingsPage()->widget(), tr("构建设置"));
//    insertTab(3, ValgrindPlugin::valgrindOpitonsPage()->widget(), tr("分析器"));





//    m_mainWidget->addTab(new QWidget(), tr("编译器"));

//    m_mainLayout->addWidget(m_modeWidget);
//    //ok
//    m_okLayout = new QHBoxLayout();
//    m_okLayout->addStretch();
//    m_okBtn = new QPushButton(tr("应用"));
//    m_okLayout->addWidget(m_okBtn);
//    m_okLayout->addStretch();
//    m_mainLayout->addLayout(m_okLayout);
//    connect(m_okBtn, SIGNAL(clicked()), this, SLOT(slotSaveSettings()));

//    QWidget *frame = new QWidget();
//    frame->setLayout(m_mainLayout);
    setWidget(m_modeWidget);
}

void SyberOSDeviceMode::sybersdkPluginAdded(QObject *obj)
{
    Q_UNUSED(obj)
}

void SyberOSDeviceMode::slotSaveSettings()
{
    Core::ICore::saveSettings(Core::ICore::InitializationDone); // save all settings
}


SyberOSDevicePlugin::SyberOSDevicePlugin()
{
    // Create your members
}

SyberOSDevicePlugin::~SyberOSDevicePlugin()
{
    if(m_syberOSDeviceMode != nullptr){
        delete m_syberOSDeviceMode;
        m_syberOSDeviceMode = nullptr;
    }
}

bool SyberOSDevicePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    m_syberOSDeviceMode = new SyberOSDeviceMode;
    return true;
}

void SyberOSDevicePlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag SyberOSDevicePlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void SyberOSDevicePlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action triggered"),
                             tr("This is an action from SyberOSDevice."));
}


ModelTabWidget::ModelTabWidget(QWidget *parent):QTabWidget(parent)
{

}

void ModelTabWidget::showEvent(QShowEvent *event)
{
    QTabWidget::showEvent(event);
}

void ModelTabWidget::hideEvent(QHideEvent *event)
{
    QTabWidget::hideEvent(event);
}

void ModelTabWidget::slotSettingFinish()
{
//    insertTab(0, ProjectExplorerPlugin::instance()->deviceSettingsPage()->widget(), tr("设备管理"));
//    insertTab(1 ,ProjectExplorerPlugin::instance()->kitOptionPage()->widget(), tr("构建套件(Kit)"));
//    insertTab(2, ProjectExplorerPlugin::instance()->projectExplorerSettingsPage()->widget(), tr("构建设置"));
//    insertTab(3, ValgrindPlugin::valgrindOpitonsPage()->widget(), tr("分析器"));
}

} // namespace Internal
} // namespace SyberOSDevice
