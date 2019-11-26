#pragma once

#include "sdkmanager_global.h"

#include <extensionsystem/iplugin.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <QWidget>
#include <QProcess>

namespace sdkmanager {
namespace Internal {

class SdkManagerMode : public Core::IMode
{
    Q_OBJECT
public:
    SdkManagerMode();
    ~SdkManagerMode() override;

    void startProgram();
    void initPlugins();

private slots:
    void startFinish();
    void finishProgram(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError);

private:
    void startSdkManagerInstallProgram(Core::Id mode, Core::Id oldMode);
    void killProcess();

    QWidget *m_modeWidget;
    QProcess* process = nullptr;
};

class sdkmanagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "sdkmanager.json")

public:
    sdkmanagerPlugin();
    ~sdkmanagerPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    bool delayedInitialize() override;
    ShutdownFlag aboutToShutdown() override;

private slots:
    void triggerAction();
private:
    SdkManagerMode *sdkmanagerMode = nullptr;
};


} // namespace Internal
} // namespace sdkmanager
