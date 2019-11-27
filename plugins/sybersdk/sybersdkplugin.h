#ifndef SYBERSDK_H
#define SYBERSDK_H

#include <extensionsystem/iplugin.h>

#include <coreplugin/imode.h>

#include <QQmlNetworkAccessManagerFactory>
#include <QQuickView>
#include <QQuickWindow>

#include "developermanager.h"

#include <utils/networkaccessmanager.h>


namespace SyberSDK {
namespace Internal {

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory();
    QNetworkAccessManager* create(QObject *parent);
};


class SyberSDKMode : public Core::IMode
{
    Q_OBJECT
    Q_PROPERTY(int activePlugin READ activePlugin WRITE setActivePlugin NOTIFY activePluginChanged)
public:
    SyberSDKMode();
    ~SyberSDKMode();

    void initPlugins();
    int activePlugin();

public slots:
    void setActivePlugin(int pos);
signals:
    void activePluginChanged(int pos);

private slots:
    void sybersdkPluginAdded(QObject*);
    void sceneGraphError(QQuickWindow::SceneGraphError, const QString &message);

private:
    void facilitateQml(QQmlEngine *engine);

    QWidget *m_modeWidget;
    QQuickView *m_syberSDKPage;
    QList<QObject*> m_pluginList;
    int m_activePlugin;
    DeveloperManager *m_developermanager;
    NetworkAccessManagerFactory *m_networkAccessManagerFactory;
};


class SyberSDKPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "SyberSDK.json")

public:
    SyberSDKPlugin();

    virtual bool initialize(const QStringList &arguments, QString *errorMessage);
    virtual void extensionsInitialized();

private:
    SyberSDKMode *m_SyberSDKMode = nullptr;
};

} // namespace Internal
} // namespace SyberSDK

#endif // SYBERSDK_H
