#include "sybersdkplugin.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/iwizardfactory.h>
#include <coreplugin/editormanager/editormanager.h>

#include <projectexplorer/projectexplorer.h>

#include <utils/fileutils.h>
#include <utils/hostosinfo.h>
#include <utils/styledbar.h>
#include <utils/qtcassert.h>
#include <coreplugin//iwelcomepage.h>
#include <utils/networkaccessmanager.h>

#include <QScrollArea>
#include <QDesktopServices>
#include <QPainter>
#include <QVBoxLayout>
#include <QMessageBox>

#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QUrl>
#include <QtPlugin>
#include <QTranslator>

#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlNetworkAccessManagerFactory>

#include <QtQml>
#include "sybersdkconstants.h"
#include "chooselist.h"
#include "chooselist_forbuild.h"

enum { debug = 1 };

using namespace Core;
using namespace ExtensionSystem;
using namespace Utils;

static const char currentPageSettingsKeyC[] = "SyberSDKTab";

namespace SyberSDK {
namespace Internal {

static QString applicationDirPath()
{
    // normalize paths so QML doesn't freak out if it's wrongly capitalized on Windows
    return Utils::FileUtils::normalizePathName(QCoreApplication::applicationDirPath());
}

static QString resourcePath()
{
    // normalize paths so QML doesn't freak out if it's wrongly capitalized on Windows
    return Utils::FileUtils::normalizePathName(Core::ICore::resourcePath());
}

NetworkAccessManagerFactory::NetworkAccessManagerFactory(): QQmlNetworkAccessManagerFactory() {}

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent) { return new Utils::NetworkAccessManager(parent); }



SyberSDKMode::SyberSDKMode() :
    m_activePlugin(0), m_developermanager(new DeveloperManager)
    , m_networkAccessManagerFactory(new NetworkAccessManagerFactory)
{

    setDisplayName(tr("证书"));
    QIcon qtLogo;
    qtLogo.addFile(QLatin1String(Constants::SYBER_32_ICON));
    qtLogo.addFile(QLatin1String(Constants::SYBER_64_ICON));
    qtLogo.addFile(QLatin1String(Constants::SYBER_128_ICON));
    setIcon(qtLogo);
    setPriority(1); //THe lowest priority
    setId("SyberSDK");
    setContextHelp("Qt Creator SyberSDK");
    setContext(Core::Context("Syber.SyberSDKMode"));

    m_syberSDKPage = new QQuickView;

    connect(m_syberSDKPage, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError,QString)),
            this, SLOT(sceneGraphError(QQuickWindow::SceneGraphError,QString)));

    m_syberSDKPage->setObjectName(QLatin1String("SyberSDKPage"));
    m_syberSDKPage->setResizeMode(QQuickView::SizeRootObjectToView);

//  filter to forward dragEnter events
//    m_syberSDKPage->installEventFilter(this);
//    m_syberSDKPage->viewport()->installEventFilter(this);

    m_modeWidget = new QWidget;
    m_modeWidget->setObjectName(QLatin1String("SyberSDKPageModeWidget"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    Utils::StyledBar* styledBar = new Utils::StyledBar(m_modeWidget);
    styledBar->setObjectName(QLatin1String("SyberSDKPageStyledBar"));
    layout->addWidget(styledBar);

    QWidget *container = QWidget::createWindowContainer(m_syberSDKPage, m_modeWidget);
    m_modeWidget->setLayout(layout);
    layout->addWidget(container);

    connect(PluginManager::instance(), SIGNAL(objectAdded(QObject*)), SLOT(sybersdkPluginAdded(QObject*)));

    setWidget(m_modeWidget);
}

SyberSDKMode::~SyberSDKMode()
{
    QSettings *settings = Core::ICore::settings();
    settings->setValue(QLatin1String(currentPageSettingsKeyC), activePlugin());
    delete m_modeWidget;
    delete m_developermanager;
    delete m_networkAccessManagerFactory;
}

void SyberSDKMode::sceneGraphError(QQuickWindow::SceneGraphError, const QString &message)
{
    QMessageBox *messageBox =
        new QMessageBox(QMessageBox::Warning,
                        tr("SyberSDK Mode Load Error"), message,
                        QMessageBox::Close, m_modeWidget);
    messageBox->setModal(false);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->show();
}

void SyberSDKMode::facilitateQml(QQmlEngine *)
{
}

void SyberSDKMode::initPlugins()
{
    // If ~/.config/SyberSDK/SyberSDK doesn't include 'SyberSDKTab', this will set 0 to it.
    QSettings *settings = Core::ICore::settings();
    setActivePlugin(settings->value(QLatin1String(currentPageSettingsKeyC)).toInt());

    QQmlContext *ctx = m_syberSDKPage->rootContext();
    ctx->setContextProperty(QLatin1String("SyberSDKMode"), this);
    ctx->setContextProperty(QLatin1String("develmanager"), m_developermanager);
    qmlRegisterType<ObjectListModel>("ObjectListModel", 1, 0, "ObjectListModel");
    qmlRegisterType<Chooselist>("Chooselist",1,0,"Chooselist");
    qmlRegisterType<Chooselist_forbuild>("Chooselist_forbuild",1,0,"Chooselist_forbuild");
//    QList<Utils::IWelcomePage*> duplicatePlugins = PluginManager::getObjects<Utils::IWelcomePage>();
//    qSort(duplicatePlugins.begin(), duplicatePlugins.end(), &sortFunction);

//    QList<Utils::IWelcomePage*> plugins;
//    QHash<Utils::IWelcomePage::Id, Utils::IWelcomePage*> pluginHash;

//    //avoid duplicate ids - choose by priority
//    foreach (Utils::IWelcomePage* plugin, duplicatePlugins) {
//        if (pluginHash.contains(plugin->id())) {
//            Utils::IWelcomePage* pluginOther = pluginHash.value(plugin->id());

//            if (pluginOther->priority() > plugin->priority()) {
//                plugins.removeAll(pluginOther);
//                pluginHash.remove(pluginOther->id());
//                plugins << plugin;
//                pluginHash.insert(plugin->id(), plugin);
//            }

//        } else {
//            plugins << plugin;
//            pluginHash.insert(plugin->id(), plugin);
//        }
//    }

    QQmlEngine *engine = m_syberSDKPage->engine();
    QStringList importPathList = engine->importPathList();
    importPathList << resourcePath() + QLatin1String("/syberos");
    engine->setImportPathList(importPathList);
    if (!debug)
        engine->setOutputWarningsToStandardError(false);
    engine->setNetworkAccessManagerFactory(m_networkAccessManagerFactory);
    QString pluginPath = applicationDirPath();
    if (HostOsInfo::isMacHost())
        pluginPath += QLatin1String("/../PlugIns");
    else
        pluginPath += QLatin1String("/../" IDE_LIBRARY_BASENAME "/qtcreator");
    engine->addImportPath(QDir::cleanPath(pluginPath));
    facilitateQml(engine);

//    foreach (Utils::IWelcomePage *plugin, plugins) {
//        plugin->facilitateQml(engine);
//        m_pluginList.append(plugin);
//    }
//    ctx->setContextProperty(QLatin1String("pagesModel"), QVariant::fromValue(m_pluginList));


    QString path = resourcePath() + QLatin1String("/syberos/syberos.qml");

    // finally, load the root page
    m_syberSDKPage->setSource(
            QUrl::fromLocalFile(path));
}

//QString SyberSDKMode::platform() const
//{
//    switch (HostOsInfo::hostOs()) {
//    case OsTypeWindows: return QLatin1String("windows");
//    case OsTypeMac: return QLatin1String("mac");
//    case OsTypeLinux: return QLatin1String("linux");
//    case OsTypeOtherUnix: return QLatin1String("unix");
//    default: return QLatin1String("other");
//    }
//}

int SyberSDKMode::activePlugin() { return m_activePlugin; }


void SyberSDKMode::setActivePlugin(int pos)
{
    if (m_activePlugin != pos) {
        m_activePlugin = pos;
        emit activePluginChanged(pos);
    }
}

void SyberSDKMode::sybersdkPluginAdded(QObject *obj)
{
    Q_UNUSED(obj)
}


SyberSDKPlugin::SyberSDKPlugin()
{
}

/*! Initializes the plugin. Returns true on success.
    Plugins want to register objects with the plugin manager here.

    \a errorMessage can be used to pass an error message to the plugin system,
       if there was any.
*/
bool SyberSDKPlugin::initialize(const QStringList & /* arguments */, QString * /* errorMessage */)
{
//    QTranslator* translator = new QTranslator(this);
//    translator->load(QLatin1String("./sybersdk_zh_CN.qm"));
//    qApp->installTranslator(translator);

    m_SyberSDKMode = new SyberSDKMode;

    return true;
}

/*! Notification that all extensions that this plugin depends on have been
    initialized. The dependencies are defined in the plugins .qwp file.

    Normally this function is used for things that rely on other plugins to have
    added objects to the plugin manager, that implement interfaces that we're
    interested in. These objects can now be requested through the
    PluginManagerInterface.

    The SyberSDKPlugin doesn't need things from other plugins, so it does
    nothing here.
*/
void SyberSDKPlugin::extensionsInitialized()
{
    m_SyberSDKMode->initPlugins();

    //Initialize the plugin to be shown when qtcreator starts.
    //Core::ModeManager::activateMode(m_SyberSDKMode->id());
}


} // namespace Internal
} // namespace SyberSDK
