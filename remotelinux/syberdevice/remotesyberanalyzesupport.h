
#ifndef REMOTESYBERANALYZESUPPORT_H
#define REMOTESYBERANALYZESUPPORT_H

#include "abstractremotelinuxrunsupport.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/runconfiguration.h>

#include <utils/outputformat.h>

namespace Debugger { class AnalyzerRunControl; }

namespace RemoteLinux {

namespace Internal { class RemoteSyberAnalyzeSupportPrivate; }

class REMOTELINUX_EXPORT RemoteSyberAnalyzeSupport : public AbstractRemoteLinuxRunSupport
{
    Q_OBJECT
public:
    RemoteSyberAnalyzeSupport(ProjectExplorer::RunConfiguration *runConfig,
            Debugger::AnalyzerRunControl *engine, Core::Id runMode);
    ~RemoteSyberAnalyzeSupport();

protected:
    void startExecution();
    void handleAdapterSetupFailed(const QString &error);

private slots:
    void handleRemoteSetupRequested();
    void handleAppRunnerError(const QString &error);
    void handleRemoteOutput(const QByteArray &output);
    void handleRemoteErrorOutput(const QByteArray &output);
    void handleAppRunnerFinished(bool success);
    void handleProgressReport(const QString &progressOutput);

    void handleRemoteProcessStarted();
    void handleProfilingFinished();

    void remoteIsRunning();

private:
    void showMessage(const QString &, Utils::OutputFormat);

    Internal::RemoteSyberAnalyzeSupportPrivate * const d;
};

} // namespace RemoteLinux

#endif // REMOTESYBERANALYZESUPPORT_H
