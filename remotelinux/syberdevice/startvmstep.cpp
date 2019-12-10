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
#include "startvmstep.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>

using namespace ProjectExplorer;
static const char INITSTARVM_ARGUMENTS_KEY[] = "RemoteLinux.StartVMStep.Arguments";

namespace RemoteLinux {

StartVMStep::StartVMStep(BuildStepList *bsl)
    : AbstractProcessStep(bsl, stepId())
{
    ctor();
}

StartVMStep::StartVMStep(BuildStepList *bsl, StartVMStep *other)
    : AbstractProcessStep(bsl, other)
{
    ctor();
}

void StartVMStep::ctor()
{
    setDefaultDisplayName(displayName());
}

StartVMStep::~StartVMStep()
{
}

bool StartVMStep::init(QList<const BuildStep *> &earlierSteps)
{
    BuildConfiguration* bc = target()->activeBuildConfiguration();
    Q_ASSERT(bc != NULL);

    ProcessParameters *pp = processParameters();
    pp->setEnvironment(bc->environment());
    pp->setCommand(bc->environment().value(QLatin1String("STARTVM")));

    if (!m_osVersionArgs.isEmpty())
        pp->setArguments(m_osVersionArgs);

    return AbstractProcessStep::init(earlierSteps);
}

QVariantMap StartVMStep::toMap() const
{
    QVariantMap map(AbstractProcessStep::toMap());
    map.insert(QLatin1String(INITSTARVM_ARGUMENTS_KEY), m_osVersionArgs);
    return map;
}

bool StartVMStep::fromMap(const QVariantMap &map)
{
    m_osVersionArgs = map.value(QLatin1String(INITSTARVM_ARGUMENTS_KEY)).toString();
    return AbstractProcessStep::fromMap(map);
}

void StartVMStep::setOsArguments(const QString &arguments)
{
    m_osVersionArgs = arguments;
}

void StartVMStep::run(QFutureInterface<bool> &fi)
{
    AbstractProcessStep::run(fi);
}

BuildStepConfigWidget *StartVMStep::createConfigWidget()
{
    return new SimpleBuildStepConfigWidget(this);
}

Core::Id StartVMStep::stepId()
{
    return "MaemoStartVMStep";
}

QString StartVMStep::displayName()
{
    return tr("Start VM");
}

} // namespace RemoteLinux
