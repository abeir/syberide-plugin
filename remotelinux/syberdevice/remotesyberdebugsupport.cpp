#include "remotesyberdebugsupport.h"

#include "remotelinuxrunconfiguration.h"

#include <debugger/debuggerrunconfigurationaspect.h>
#include <debugger/debuggerruncontrol.h>
#include <debugger/debuggerstartparameters.h>
#include <debugger/debuggerkitinformation.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/projectinformation.h>
#include <projectexplorer/devicesupport/deviceapplicationrunner.h>
#include <projectexplorer/project.h>
#include <projectexplorer/runnables.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>

#include <qmldebug/qmldebugcommandlinearguments.h>

#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>

#include <QPointer>

using namespace QSsh;
using namespace Debugger;
using namespace ProjectExplorer;
using namespace Utils;

namespace RemoteLinux {
namespace Internal {

class SyberDeviceDebugSupportPrivate
{
public:
    SyberDeviceDebugSupportPrivate(const RunConfiguration *runConfig, DebuggerRunControl *runControl)
        : runControl(runControl),
          qmlDebugging(runConfig->extraAspect<DebuggerRunConfigurationAspect>()->useQmlDebugger()),
          cppDebugging(runConfig->extraAspect<DebuggerRunConfigurationAspect>()->useCppDebugger()),
          gdbServerPort(-1), qmlPort(-1)
    {
    }

    const QPointer<DebuggerRunControl> runControl;
    bool qmlDebugging;
    bool cppDebugging;
    QByteArray gdbserverOutput;
    int gdbServerPort;
    int qmlPort;
};

} // namespace Internal

using namespace Internal;

SyberDeviceDebugSupport::SyberDeviceDebugSupport(RunConfiguration *runConfig,
        DebuggerRunControl *runControl)
    : AbstractRemoteLinuxRunSupport(runConfig, runControl),
      d(new SyberDeviceDebugSupportPrivate(runConfig, runControl))
{
    connect(runControl, &DebuggerRunControl::requestRemoteSetup,
            this, &SyberDeviceDebugSupport::handleRemoteSetupRequested);
    connect(runControl, &RunControl::finished,
            this, &SyberDeviceDebugSupport::handleDebuggingFinished);
}

SyberDeviceDebugSupport::~SyberDeviceDebugSupport()
{
    delete d;
}

void SyberDeviceDebugSupport::showMessage(const QString &msg, int channel)
{
    if (state() != Inactive && d->runControl)
        d->runControl->showMessage(msg, channel);
}

void SyberDeviceDebugSupport::handleRemoteSetupRequested()
{
    QTC_ASSERT(state() == Inactive, return);

    showMessage(tr("Checking available ports...") + QLatin1Char('\n'), LogStatus);
    AbstractRemoteLinuxRunSupport::handleRemoteSetupRequested();
}

void SyberDeviceDebugSupport::startExecution()
{
    QTC_ASSERT(state() == GatheringPorts, return);

    if (d->cppDebugging && !setPort(d->gdbServerPort))
        return;
    if (d->qmlDebugging && !setPort(d->qmlPort))
            return;

    setState(StartingRunner);
    d->gdbserverOutput.clear();

    DeviceApplicationRunner *runner = appRunner();
    connect(runner, &DeviceApplicationRunner::remoteStderr,
            this, &SyberDeviceDebugSupport::handleRemoteErrorOutput);
    connect(runner, &DeviceApplicationRunner::remoteStdout,
            this, &SyberDeviceDebugSupport::handleRemoteOutput);
    connect(runner, &DeviceApplicationRunner::finished,
            this, &SyberDeviceDebugSupport::handleAppRunnerFinished);
    connect(runner, &DeviceApplicationRunner::reportProgress,
            this, &SyberDeviceDebugSupport::handleProgressReport);
    connect(runner, &DeviceApplicationRunner::reportError,
            this, &SyberDeviceDebugSupport::handleAppRunnerError);
    if (d->qmlDebugging && !d->cppDebugging)
        connect(runner, &DeviceApplicationRunner::remoteProcessStarted,
                this, &SyberDeviceDebugSupport::handleRemoteProcessStarted);

    StandardRunnable r = runnable();
    QStringList args = QtcProcess::splitArgs(r.commandLineArguments, OsTypeLinux);
    QString command;

    if (d->qmlDebugging)
        args.prepend(QString::fromLatin1("-qmljsdebugger=port:%1,block").arg(d->qmlPort));

    QString runInfo = ProjectInformation::instance().curProjectRunExecInfo(r.executable);
    if(!runInfo.isEmpty())
    {
        command += QLatin1String("rm -rf /data/developer/.sdk_pid; ");
        if (d->qmlDebugging && !d->cppDebugging) {
            //motified by wjm
            command += ProjectInformation::instance().remoteFullExecCommand(r.executable);
        }
        else
        {
            command += ProjectInformation::instance().remoteFullExecCommand(r.executable, 3);
            args += QString::fromLatin1(
                            "& while [[ ! -f /data/developer/.sdk_pid ]];"
                            " do"
                            " sleep 0.1;"
                            " done;"
                            " pid=`cat /data/developer/.sdk_pid`; "
                            " gdbserver --once :%1 --attach $pid;").arg(d->gdbServerPort);
        }
    }
    else
    {
        if (d->qmlDebugging && !d->cppDebugging) {
            command = ProjectInformation::instance().remoteFullExecCommand(r.executable);
        } else {
            command = device()->debugServerPath();
            if (command.isEmpty())
                command = QLatin1String("gdbserver");
            command = ProjectInformation::instance().remoteFullExecCommand(command);
            args.clear();
            args.append(QString::fromLatin1("--multi"));
            args.append(QString::fromLatin1(":%1").arg(d->gdbServerPort));
            args.append(r.executable);
        }
    }

    r.executable = command;
    QString argument;
    foreach(const QString&str, args)
    {
        argument += QLatin1String(" ");
        argument += str;
    }
    r.commandLineArguments = argument;
    runner->start(device(), r);

    // the runnable is different from start command
    runner->setRunnable(runnable());
}

void SyberDeviceDebugSupport::handleAppRunnerError(const QString &error)
{
    if (state() == Running) {
        showMessage(error, AppError);
        if (d->runControl)
            d->runControl->notifyInferiorIll();
    } else if (state() != Inactive) {
        handleAdapterSetupFailed(error);
    }
}

void SyberDeviceDebugSupport::handleAppRunnerFinished(bool success)
{
    if (!d->runControl || state() == Inactive)
        return;

    if (state() == Running) {
        // The QML engine does not realize on its own that the application has finished.
        if (d->qmlDebugging && !d->cppDebugging)
            d->runControl->quitDebugger();
        else if (!success)
            d->runControl->notifyInferiorIll();

    } else if (state() == StartingRunner) {
        RemoteSetupResult result;
        result.success = false;
        result.reason = tr("Debugging failed.");
        d->runControl->notifyEngineRemoteSetupFinished(result);
    }
    reset();
}

void SyberDeviceDebugSupport::handleDebuggingFinished()
{
    setFinished();
    reset();
}

void SyberDeviceDebugSupport::handleRemoteOutput(const QByteArray &output)
{
    QTC_ASSERT(state() == Inactive || state() == Running, return);

    showMessage(QString::fromUtf8(output), AppOutput);
}

void SyberDeviceDebugSupport::handleRemoteErrorOutput(const QByteArray &output)
{
    QTC_ASSERT(state() != GatheringPorts, return);

    if (!d->runControl)
        return;

    showMessage(QString::fromUtf8(output), AppError);
    if (state() == StartingRunner && d->cppDebugging) {
        d->gdbserverOutput += output;
        if (d->gdbserverOutput.contains("Listening on port")) {
            handleAdapterSetupDone();
            d->gdbserverOutput.clear();
        }
    }
}

void SyberDeviceDebugSupport::handleProgressReport(const QString &progressOutput)
{
    showMessage(progressOutput + QLatin1Char('\n'), LogStatus);
}

void SyberDeviceDebugSupport::handleAdapterSetupFailed(const QString &error)
{
    AbstractRemoteLinuxRunSupport::handleAdapterSetupFailed(error);

    RemoteSetupResult result;
    result.success = false;
    result.reason = tr("Initial setup failed: %1").arg(error);
    d->runControl->notifyEngineRemoteSetupFinished(result);
}

void SyberDeviceDebugSupport::handleAdapterSetupDone()
{
    AbstractRemoteLinuxRunSupport::handleAdapterSetupDone();

    RemoteSetupResult result;
    result.success = true;
    result.gdbServerPort = d->gdbServerPort;
    result.qmlServerPort = d->qmlPort;
    d->runControl->notifyEngineRemoteSetupFinished(result);
}

void SyberDeviceDebugSupport::handleRemoteProcessStarted()
{
    QTC_ASSERT(d->qmlDebugging && !d->cppDebugging, return);
    QTC_ASSERT(state() == StartingRunner, return);

    handleAdapterSetupDone();
}

} // namespace RemoteLinux
