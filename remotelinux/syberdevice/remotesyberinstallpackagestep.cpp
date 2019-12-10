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
#include "remotesyberinstallpackagestep.h"
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildconfiguration.h>

#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

using namespace ProjectExplorer;

namespace RemoteLinux {
namespace Internal {
namespace {

const char CommandLineKey[] = "RemoteLinuxInstallPackageStep.CommandLine";

class ConfigWidget : public SimpleBuildStepConfigWidget
{
    Q_OBJECT
public:
    ConfigWidget(AbstractRemoteSyberInstallPackageStep *step)
        : SimpleBuildStepConfigWidget(step)
    {
        QVBoxLayout * const mainLayout = new QVBoxLayout(this);
        mainLayout->setMargin(0);
        QHBoxLayout * const commandLineLayout = new QHBoxLayout;
        mainLayout->addLayout(commandLineLayout);
        QLabel * const commandLineLabel = new QLabel(tr("Command line:"));
        commandLineLayout->addWidget(commandLineLabel);
        m_commandLineEdit.setText(step->commandLine());
        commandLineLayout->addWidget(&m_commandLineEdit);

        connect(&m_commandLineEdit, SIGNAL(textEdited(QString)), SLOT(handleCommandLineEdited()));
    }

    bool showWidget() const { return true; }

private:
    Q_SLOT void handleCommandLineEdited() {
        AbstractRemoteSyberInstallPackageStep *step =
            qobject_cast<AbstractRemoteSyberInstallPackageStep *>(this->step());
        step->setCommandLine(m_commandLineEdit.text().trimmed());
    }

    QLineEdit m_commandLineEdit;
};

} // anonymous namespace

class AbstractRemoteLinuxInstallPackageStepPrivate
{
public:
    QString commandLine;

};

class RemoteLinuxInstallPackageStepPrivate
{
public:
    RemoteSyberCustomCommandDeployService service;
};

} // namespace Internal

using namespace Internal;


AbstractRemoteSyberInstallPackageStep::AbstractRemoteSyberInstallPackageStep(BuildStepList *bsl,
        const Core::Id id)
    : AbstractRemoteLinuxDeployStep(bsl, id)
{
    ctor();
}

AbstractRemoteSyberInstallPackageStep::AbstractRemoteSyberInstallPackageStep(BuildStepList *bsl,
        AbstractRemoteSyberInstallPackageStep *other)
    : AbstractRemoteLinuxDeployStep(bsl, other)
{
    ctor();
}

AbstractRemoteSyberInstallPackageStep::~AbstractRemoteSyberInstallPackageStep()
{
    delete d;
}

void AbstractRemoteSyberInstallPackageStep::ctor()
{
    d = new AbstractRemoteLinuxInstallPackageStepPrivate;
    Q_ASSERT(d != NULL);

    QString pkgfile = project()->getPackageName();
    if (!pkgfile.isEmpty())
    {
        QString installCmd;
        BuildConfiguration* bc = target()->activeBuildConfiguration();

        if (bc)
            installCmd = bc->environment().value(QLatin1String("INSTALL_SOP_COMMAND"));

        if (installCmd.isEmpty())
            installCmd = QLatin1String("/usr/bin/ins-tool -siu");

         d->commandLine = QString::fromLatin1("%1 /tmp/%2 && rm -f /tmp/%3").arg(installCmd, pkgfile, pkgfile);
    }
}

void AbstractRemoteSyberInstallPackageStep::refreshCommandLine()
{
    Q_ASSERT(d != NULL);
    QString pkgfile = project()->getPackageName();
    if (!pkgfile.isEmpty())
    {
        QString installCmd;
        BuildConfiguration* bc = target()->activeBuildConfiguration();

        if (bc)
            installCmd = bc->environment().value(QLatin1String("INSTALL_SOP_COMMAND"));

        if (installCmd.isEmpty())
            installCmd = QLatin1String("/usr/bin/ins-tool -siu");

        d->commandLine = QString::fromLatin1("%1 /tmp/%2 && rm -f /tmp/%3").arg(installCmd, pkgfile, pkgfile);
    }
}

bool AbstractRemoteSyberInstallPackageStep::fromMap(const QVariantMap &map)
{
    if (!AbstractRemoteLinuxDeployStep::fromMap(map))
        return false;
    d->commandLine = map.value(QLatin1String(CommandLineKey)).toString();
    return true;
}

QVariantMap AbstractRemoteSyberInstallPackageStep::toMap() const
{
    QVariantMap map = AbstractRemoteLinuxDeployStep::toMap();
    map.insert(QLatin1String(CommandLineKey), d->commandLine);
    return map;
}

void AbstractRemoteSyberInstallPackageStep::setCommandLine(const QString &commandLine)
{
    d->commandLine = commandLine;
}

QString AbstractRemoteSyberInstallPackageStep::commandLine() const
{
    return d->commandLine;
}

bool AbstractRemoteSyberInstallPackageStep::initInternal(QString *error)
{
    // 解决用户更改sopconfig.xml中sopid等信息后，QtCreator安装包命令没有更新的问题；
    // 这里使每次安装应用都重新更改新commandline内容。张晓伟20151231修改
    refreshCommandLine();

    deployService()->setCommandLine(d->commandLine);
    return deployService()->isDeploymentPossible(error);
}

BuildStepConfigWidget *AbstractRemoteSyberInstallPackageStep::createConfigWidget()
{
    // Hide command line
    //return new ConfigWidget(this);
    return new SimpleBuildStepConfigWidget(this);
}


RemoteSyberInstallPackageStep::RemoteSyberInstallPackageStep(BuildStepList *bsl)
    : AbstractRemoteSyberInstallPackageStep(bsl, stepId())
{
    ctor();
}

RemoteSyberInstallPackageStep::RemoteSyberInstallPackageStep(BuildStepList *bsl,
        RemoteSyberInstallPackageStep *other)
    : AbstractRemoteSyberInstallPackageStep(bsl, other)
{
    ctor();
}

RemoteSyberInstallPackageStep::~RemoteSyberInstallPackageStep()
{
    delete d;
}

void RemoteSyberInstallPackageStep::ctor()
{
    d = new RemoteLinuxInstallPackageStepPrivate;
    setDefaultDisplayName(displayName());
}

RemoteSyberCustomCommandDeployService *RemoteSyberInstallPackageStep::deployService() const
{
    return &d->service;
}

Core::Id RemoteSyberInstallPackageStep::stepId()
{
    return "RemoteLinux.RemoteSyberInstallPackageStep";
}

QString RemoteSyberInstallPackageStep::displayName()
{
    return tr("Install the package");
}

} // namespace RemoteLinux

