
#ifndef REMOTESYBERDEBUGSUPPORT_H
#define REMOTESYBERDEBUGSUPPORT_H

#include "abstractremotelinuxrunsupport.h"

namespace Debugger { class DebuggerRunControl; }

namespace RemoteLinux {

namespace Internal { class SyberDeviceDebugSupportPrivate; }

class REMOTELINUX_EXPORT SyberDeviceDebugSupport : public AbstractRemoteLinuxRunSupport
{
    Q_OBJECT

public:
    SyberDeviceDebugSupport(ProjectExplorer::RunConfiguration *runConfig,
            Debugger::DebuggerRunControl *runControl);
    ~SyberDeviceDebugSupport();

protected:
    void startExecution();
    void handleAdapterSetupFailed(const QString &error);
    void handleAdapterSetupDone();

private slots:
    void handleRemoteSetupRequested();
    void handleAppRunnerError(const QString &error);
    void handleRemoteOutput(const QByteArray &output);
    void handleRemoteErrorOutput(const QByteArray &output);
    void handleAppRunnerFinished(bool success);
    void handleProgressReport(const QString &progressOutput);

    void handleRemoteProcessStarted();
    void handleDebuggingFinished();

private:
    void showMessage(const QString &msg, int channel);

    Internal::SyberDeviceDebugSupportPrivate * const d;
};

} // namespace RemoteLinux

#endif // REMOTESYBERDEBUGSUPPORT_H
