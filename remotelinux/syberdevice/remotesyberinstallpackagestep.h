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
#ifndef REMOTESYBERINSTALLPACKAGESTEP_H
#define REMOTESYBERINSTALLPACKAGESTEP_H

#include "abstractremotelinuxdeploystep.h"
#include "remotesybercustomcommanddeployservice.h"

namespace RemoteLinux {
namespace Internal {
class AbstractRemoteLinuxInstallPackageStepPrivate;
class RemoteLinuxInstallPackageStepPrivate;
} // namespace Internal


class REMOTELINUX_EXPORT AbstractRemoteSyberInstallPackageStep
    : public AbstractRemoteLinuxDeployStep
{
    Q_OBJECT
public:
    ~AbstractRemoteSyberInstallPackageStep();

    bool fromMap(const QVariantMap &map);
    QVariantMap toMap() const;

    void setCommandLine(const QString &commandLine);
    QString commandLine() const;

protected:
    AbstractRemoteSyberInstallPackageStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id);
    AbstractRemoteSyberInstallPackageStep(ProjectExplorer::BuildStepList *bsl, AbstractRemoteSyberInstallPackageStep *other);

    bool initInternal(QString *error = 0);

private:
    void ctor();
   void refreshCommandLine();

    RemoteSyberCustomCommandDeployService *deployService() const = 0;
    ProjectExplorer::BuildStepConfigWidget *createConfigWidget();

    Internal::AbstractRemoteLinuxInstallPackageStepPrivate *d;

};


class REMOTELINUX_EXPORT RemoteSyberInstallPackageStep
    : public AbstractRemoteSyberInstallPackageStep
{
    Q_OBJECT
public:
    RemoteSyberInstallPackageStep(ProjectExplorer::BuildStepList *bsl);
    RemoteSyberInstallPackageStep(ProjectExplorer::BuildStepList *bsl, RemoteSyberInstallPackageStep *other);
    ~RemoteSyberInstallPackageStep();

    static Core::Id stepId();
    static QString displayName();

private:
    RemoteSyberCustomCommandDeployService *deployService() const;
    void ctor();

    Internal::RemoteLinuxInstallPackageStepPrivate *d;
};

} // namespace RemoteLinux

#endif // REMOTESYBERINSTALLPACKAGESTEP_H
