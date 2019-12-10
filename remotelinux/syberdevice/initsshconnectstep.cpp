/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/
#include "initsshconnectstep.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitinformation.h>
#include <ssh/sshconnection.h>

using namespace ProjectExplorer;

static const char INITSSHCONNECT_ARGUMENTS_KEY[] = "RemoteLinux.InitSshConnectStep.Arguments";

namespace RemoteLinux {

InitSshConnectStep::InitSshConnectStep(BuildStepList *bsl)
    : AbstractProcessStep(bsl, stepId())
{
    ctor();
}

InitSshConnectStep::InitSshConnectStep(BuildStepList *bsl, InitSshConnectStep *other)
    : AbstractProcessStep(bsl, other)
{
    ctor();
}

void InitSshConnectStep::ctor()
{
    setDefaultDisplayName(displayName());
}

InitSshConnectStep::~InitSshConnectStep()
{
}

bool InitSshConnectStep::init(QList<const BuildStep *> &earlierSteps)
{
    BuildConfiguration* bc = target()->activeBuildConfiguration();
    Q_ASSERT(bc != NULL);

    ProcessParameters *pp = processParameters();
    pp->setEnvironment(bc->environment());
    pp->setCommand(bc->environment().value(QLatin1String("INITSSHCONNECT")));

    IDevice::ConstPtr p_dev = DeviceKitInformation::device(target()->kit());
    QSsh::SshConnectionParameters sshParams = p_dev->sshParameters();
    if(sshParams.password.isEmpty())
        pp->setArguments(QString::fromLatin1("%1 %2 %3 %4").arg(sshParams.host, sshParams.userName, QLatin1String("system"),QString::number(sshParams.port)));
 //   else
 //       pp->setArguments(QString::fromLatin1("%1 %2 %3 %4").arg(sshParams.host, sshParams.userName, sshParams.password,QString::number(sshParams.port)));

    return AbstractProcessStep::init(earlierSteps);
}

QVariantMap InitSshConnectStep::toMap() const
{
    QVariantMap map(AbstractProcessStep::toMap());
    return map;
}

bool InitSshConnectStep::fromMap(const QVariantMap &map)
{
    return AbstractProcessStep::fromMap(map);
}

void InitSshConnectStep::run(QFutureInterface<bool> &fi)
{
    AbstractProcessStep::run(fi);
}

BuildStepConfigWidget *InitSshConnectStep::createConfigWidget()
{
    return new SimpleBuildStepConfigWidget(this);
}

Core::Id InitSshConnectStep::stepId()
{
    return "MaemoInitSshConnectStep";
}

QString InitSshConnectStep::displayName()
{
    return tr("Initialize connect to the device");
}

} // namespace RemoteLinux
