#include "syberosversionplugin.h"
#include "syberosversionconstants.h"
#include "versionwidget.h"

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
#include <QtPlugin>

namespace SyberOSVersion {
namespace Internal {


SyberOSVersionPlugin::SyberOSVersionPlugin()
{
    // Create your members
}

SyberOSVersionPlugin::~SyberOSVersionPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool SyberOSVersionPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    QAction *action = new QAction(tr("版本信息"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    connect(action, SIGNAL(triggered()), this, SLOT(slotShowSDKVersion()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("关于SyberOS SDK"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->addMenu(menu);

    return true;
}

void SyberOSVersionPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag SyberOSVersionPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void SyberOSVersionPlugin::slotShowSDKVersion()
{
    VersionWidget widget(Core::ICore::mainWindow());
    widget.exec();
}


} // namespace Internal
} // namespace SyberOSVersion
