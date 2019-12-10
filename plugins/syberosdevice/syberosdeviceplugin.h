#ifndef SYBEROSDEVICE_H
#define SYBEROSDEVICE_H

#include "syberosdevice_global.h"
#include "monitor/monitorwidget.h"

#include <extensionsystem/iplugin.h>
#include <coreplugin/imode.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QtQuick/QQuickView>

namespace SyberOSDevice {
namespace Internal {

class SyberOSDeviceMode : public Core::IMode
{
    Q_OBJECT
public:
    SyberOSDeviceMode();
    ~SyberOSDeviceMode();

    void activated();
    void initPlugins();

    Q_SCRIPTABLE QString platform() const;

private:
    void createLayout(void);

private slots:
    void sybersdkPluginAdded(QObject*);
    void slotSaveSettings(void);
#if QT_VERSION >= 0x050300
    void sceneGraphError(QQuickWindow::SceneGraphError, const QString &message);
#endif
private:
    QVBoxLayout *m_mainLayout = nullptr;
    QHBoxLayout *m_okLayout = nullptr;
    QPushButton *m_okBtn = nullptr;
    MonitorWidget *m_modeWidget = nullptr;
};

class SyberOSDevicePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "SyberOSDevice.json")

public:
    SyberOSDevicePlugin();
    ~SyberOSDevicePlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void triggerAction();

private:
    SyberOSDeviceMode *m_syberOSDeviceMode = nullptr;
};

class ModelTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ModelTabWidget(QWidget *parent = nullptr);

private slots:
    void slotSettingFinish(void);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
};

} // namespace Internal
} // namespace SyberOSDevice

#endif // SYBEROSDEVICE_H
