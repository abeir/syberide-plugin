/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "remotesyberruncontrolfactory.h"
#include "remotesyberanalyzesupport.h"
#include "remotelinux_constants.h"
#include "remotelinuxcustomrunconfiguration.h"
#include "remotelinuxrunconfiguration.h"
#include "remotelinuxruncontrol.h"
#include "remotesyberdebugsupport.h"

#include <debugger/debuggerruncontrol.h>
#include <debugger/debuggerrunconfigurationaspect.h>
#include <debugger/debuggerstartparameters.h>

#include <debugger/analyzer/analyzermanager.h>
#include <debugger/analyzer/analyzerruncontrol.h>
#include <debugger/analyzer/analyzerstartparameters.h>

#include <qmldebug/qmldebugcommandlinearguments.h>

#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <utils/portlist.h>
#include <utils/qtcassert.h>
#include <QDebug>

using namespace Debugger;
using namespace ProjectExplorer;

namespace RemoteLinux {
namespace Internal {

RemoteSyberRunControlFactory::RemoteSyberRunControlFactory(QObject *parent)
    : IRunControlFactory(parent)
{
}

RemoteSyberRunControlFactory::~RemoteSyberRunControlFactory()
{
}

bool RemoteSyberRunControlFactory::canRun(RunConfiguration *runConfiguration, Core::Id mode) const
{
    if (mode != ProjectExplorer::Constants::NORMAL_RUN_MODE
            && mode != ProjectExplorer::Constants::DEBUG_RUN_MODE
            && mode != ProjectExplorer::Constants::DEBUG_RUN_MODE_WITH_BREAK_ON_MAIN
            && mode != ProjectExplorer::Constants::QML_PROFILER_RUN_MODE) {
        return false;
    }
    Target* target = runConfiguration->target();
    if(target)
    {
        IDevice::ConstPtr dev = DeviceKitInformation::device(target->kit());
        if(dev==NULL)
            return false;
        if(dev->type() != ProjectExplorer::Constants::GenericSyberOsType)
            return false;
    }

    const Core::Id id = runConfiguration->id();
    return runConfiguration->isEnabled()
            && (id == RemoteLinuxCustomRunConfiguration::runConfigId()
                || id.name().startsWith(RemoteLinuxRunConfiguration::IdPrefix));
}

RunControl *RemoteSyberRunControlFactory::create(RunConfiguration *runConfig, Core::Id mode,
                                                 QString *errorMessage)
{
    QTC_ASSERT(canRun(runConfig, mode), return 0);

    if (mode == ProjectExplorer::Constants::NORMAL_RUN_MODE)
        return new RemoteLinuxRunControl(runConfig);

    const auto rcRunnable = runConfig->runnable();
    QTC_ASSERT(rcRunnable.is<StandardRunnable>(), return 0);
    const auto stdRunnable = rcRunnable.as<StandardRunnable>();

    if (mode == ProjectExplorer::Constants::DEBUG_RUN_MODE
            || mode == ProjectExplorer::Constants::DEBUG_RUN_MODE_WITH_BREAK_ON_MAIN) {
        IDevice::ConstPtr dev = DeviceKitInformation::device(runConfig->target()->kit());
        if (!dev) {
            *errorMessage = tr("Cannot debug: Kit has no device.");
            return 0;
        }

        auto aspect = runConfig->extraAspect<DebuggerRunConfigurationAspect>();
        int portsUsed = aspect ? aspect->portsUsedByDebugger() : 0;
        if (portsUsed > dev->freePorts().count()) {
            *errorMessage = tr("Cannot debug: Not enough free ports available.");
            return 0;
        }

        QString symbolFile;
        if (auto rlrc = qobject_cast<RemoteLinuxRunConfiguration *>(runConfig))
            symbolFile = rlrc->localExecutableFilePath();
        if (auto rlrc = qobject_cast<RemoteLinuxCustomRunConfiguration *>(runConfig))
            symbolFile = rlrc->localExecutableFilePath();
        if (symbolFile.isEmpty()) {
            *errorMessage = tr("Cannot debug: Local executable is not set.");
            return 0;
        }

        DebuggerStartParameters params;
        params.startMode = AttachToRemoteServer;
        params.closeMode = KillAndExitMonitorAtClose;
        params.remoteSetupNeeded = true;
        params.displayName = runConfig->displayName();
        params.useContinueInsteadOfRun = true;

        if (aspect->useQmlDebugger()) {
            params.qmlServerAddress = dev->sshParameters().host;
            params.qmlServerPort = 0; // port is selected later on
        }
        if (aspect->useCppDebugger()) {
            aspect->setUseMultiProcess(true);
            params.inferior.executable = stdRunnable.executable;
            params.inferior.commandLineArguments = stdRunnable.commandLineArguments;
            if (aspect->useQmlDebugger()) {
                params.inferior.commandLineArguments.prepend(QLatin1Char(' '));
                params.inferior.commandLineArguments.prepend(QmlDebug::qmlDebugTcpArguments(QmlDebug::QmlDebuggerServices));
            }
            params.startMode = AttachToRemoteServer;
            params.remoteChannel = dev->sshParameters().host + QLatin1String(":-1");
            params.symbolFile = symbolFile;
        }
        else {
            params.startMode = AttachToRemoteServer;
        }

        DebuggerRunControl * const runControl = createDebuggerRunControl(params, runConfig, errorMessage, mode);
        if (!runControl)
            return 0;
        (void) new SyberDeviceDebugSupport(runConfig, runControl);
        return runControl;
    }

    if (mode == ProjectExplorer::Constants::QML_PROFILER_RUN_MODE) {
        auto runControl = Debugger::createAnalyzerRunControl(runConfig, mode);
        AnalyzerConnection connection;
        connection.connParams =
            DeviceKitInformation::device(runConfig->target()->kit())->sshParameters();
        connection.analyzerHost = connection.connParams.host;
        runControl->setConnection(connection);
        (void) new RemoteSyberAnalyzeSupport(runConfig, runControl, mode);
        return runControl;
    }

    QTC_CHECK(false);
    return 0;
}

} // namespace Internal
} // namespace RemoteLinux
