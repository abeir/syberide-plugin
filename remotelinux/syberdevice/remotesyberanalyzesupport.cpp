/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "remotesyberanalyzesupport.h"

#include "remotelinuxrunconfiguration.h"

#include <debugger/analyzer/analyzerruncontrol.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/projectinformation.h>
#include <projectexplorer/devicesupport/deviceapplicationrunner.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/runnables.h>

#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>
#include <qmldebug/qmloutputparser.h>
#include <qmldebug/qmldebugcommandlinearguments.h>

#include <QPointer>

using namespace QSsh;
using namespace Debugger;
using namespace ProjectExplorer;
using namespace Utils;

namespace RemoteLinux {
namespace Internal {

class RemoteSyberAnalyzeSupportPrivate
{
public:
    RemoteSyberAnalyzeSupportPrivate(AnalyzerRunControl *rc, Core::Id runMode)
        : runControl(rc),
          qmlProfiling(runMode == ProjectExplorer::Constants::QML_PROFILER_RUN_MODE),
          qmlPort(-1)
    {
    }

    const QPointer<AnalyzerRunControl> runControl;
    bool qmlProfiling;
    int qmlPort;

    QmlDebug::QmlOutputParser outputParser;
};

} // namespace Internal

using namespace Internal;

RemoteSyberAnalyzeSupport::RemoteSyberAnalyzeSupport(RunConfiguration *runConfig,
                                                     AnalyzerRunControl *engine, Core::Id runMode)
    : AbstractRemoteLinuxRunSupport(runConfig, engine),
      d(new RemoteSyberAnalyzeSupportPrivate(engine, runMode))
{
    connect(d->runControl.data(), &AnalyzerRunControl::starting,
            this, &RemoteSyberAnalyzeSupport::handleRemoteSetupRequested);
    connect(&d->outputParser, &QmlDebug::QmlOutputParser::waitingForConnectionOnPort,
            this, &RemoteSyberAnalyzeSupport::remoteIsRunning);
    connect(engine, &RunControl::finished,
            this, &RemoteSyberAnalyzeSupport::handleProfilingFinished);
}

RemoteSyberAnalyzeSupport::~RemoteSyberAnalyzeSupport()
{
    delete d;
}

void RemoteSyberAnalyzeSupport::showMessage(const QString &msg, Utils::OutputFormat format)
{
    if (state() != Inactive && d->runControl)
        d->runControl->logApplicationMessage(msg, format);
    d->outputParser.processOutput(msg);
}

void RemoteSyberAnalyzeSupport::handleRemoteSetupRequested()
{
    QTC_ASSERT(state() == Inactive, return);

    showMessage(tr("Checking available ports...") + QLatin1Char('\n'), Utils::NormalMessageFormat);
    AbstractRemoteLinuxRunSupport::handleRemoteSetupRequested();
}

void RemoteSyberAnalyzeSupport::startExecution()
{
    QTC_ASSERT(state() == GatheringPorts, return);

    // Currently we support only QML profiling
    QTC_ASSERT(d->qmlProfiling, return);

    if (!setPort(d->qmlPort))
        return;

    setState(StartingRunner);

    DeviceApplicationRunner *runner = appRunner();
    connect(runner, &DeviceApplicationRunner::remoteStderr,
            this, &RemoteSyberAnalyzeSupport::handleRemoteErrorOutput);
    connect(runner, &DeviceApplicationRunner::remoteStdout,
            this, &RemoteSyberAnalyzeSupport::handleRemoteOutput);
    connect(runner, &DeviceApplicationRunner::remoteProcessStarted,
            this, &RemoteSyberAnalyzeSupport::handleRemoteProcessStarted);
    connect(runner, &DeviceApplicationRunner::finished,
            this, &RemoteSyberAnalyzeSupport::handleAppRunnerFinished);
    connect(runner, &DeviceApplicationRunner::reportProgress,
            this, &RemoteSyberAnalyzeSupport::handleProgressReport);
    connect(runner, &DeviceApplicationRunner::reportError,
            this, &RemoteSyberAnalyzeSupport::handleAppRunnerError);

    auto r = runnable();
    if (!r.commandLineArguments.isEmpty())
        r.commandLineArguments.append(QLatin1Char(' '));
    r.commandLineArguments
            += QString::fromLatin1("-qmljsdebugger=port:%1,block").arg(d->qmlPort);

    StandardRunnable sr = ProjectInformation::instance().remoteRunnable(r);
    runner->start(device(), sr);

    // the runnable is different from start command
    runner->setRunnable(r);
}

void RemoteSyberAnalyzeSupport::handleAppRunnerError(const QString &error)
{
    if (state() == Running)
        showMessage(error, Utils::ErrorMessageFormat);
    else if (state() != Inactive)
        handleAdapterSetupFailed(error);
}

void RemoteSyberAnalyzeSupport::handleAppRunnerFinished(bool success)
{
    // reset needs to be called first to ensure that the correct state is set.
    reset();
    if (!success)
        showMessage(tr("Failure running remote process."), Utils::NormalMessageFormat);
    d->runControl->notifyRemoteFinished();
}

void RemoteSyberAnalyzeSupport::handleProfilingFinished()
{
    setFinished();
}

void RemoteSyberAnalyzeSupport::remoteIsRunning()
{
    d->runControl->notifyRemoteSetupDone(d->qmlPort);
}

void RemoteSyberAnalyzeSupport::handleRemoteOutput(const QByteArray &output)
{
    QTC_ASSERT(state() == Inactive || state() == Running, return);

    showMessage(QString::fromUtf8(output), Utils::StdOutFormat);
}

void RemoteSyberAnalyzeSupport::handleRemoteErrorOutput(const QByteArray &output)
{
    QTC_ASSERT(state() != GatheringPorts, return);

    if (!d->runControl)
        return;

    showMessage(QString::fromUtf8(output), Utils::StdErrFormat);
}

void RemoteSyberAnalyzeSupport::handleProgressReport(const QString &progressOutput)
{
    showMessage(progressOutput + QLatin1Char('\n'), Utils::NormalMessageFormat);
}

void RemoteSyberAnalyzeSupport::handleAdapterSetupFailed(const QString &error)
{
    AbstractRemoteLinuxRunSupport::handleAdapterSetupFailed(error);
    showMessage(tr("Initial setup failed: %1").arg(error), Utils::NormalMessageFormat);
}

void RemoteSyberAnalyzeSupport::handleRemoteProcessStarted()
{
    QTC_ASSERT(d->qmlProfiling, return);
    QTC_ASSERT(state() == StartingRunner, return);

    handleAdapterSetupDone();
}

} // namespace RemoteLinux
