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
#ifndef GENERICSYBERDEVICECONFIGURATIONWIZARDPAGES_H
#define GENERICSYBERDEVICECONFIGURATIONWIZARDPAGES_H

#include "remotelinux_export.h"
#include <projectexplorer/devicesupport/idevice.h>
#include <ssh/sshconnection.h>

#include <QWizardPage>

namespace RemoteLinux {
namespace Internal {
class GenericSyberDeviceConfigurationWizardSetupPagePrivate;
class GenericSyberDeviceConfigurationWizardFinalPagePrivate;
} // namespace Internal

class REMOTELINUX_EXPORT GenericSyberDeviceConfigurationWizardSetupPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit GenericSyberDeviceConfigurationWizardSetupPage(QWidget *parent = 0);
    ~GenericSyberDeviceConfigurationWizardSetupPage();

    void initializePage();
    bool isComplete() const;
    int sshPort() const;

    QString configurationName() const;
    QString hostName() const;
    QString userName() const;
    QSsh::SshConnectionParameters::AuthenticationType authenticationType() const;
    QString password() const;
    QString privateKeyFilePath() const;
    QString osVersion() const;
    QSize screenSize() const;
    ProjectExplorer::IDevice::MachineType machineType() const;

    QString defaultConfigurationName(ProjectExplorer::IDevice::MachineType);
    QString defaultHostName(ProjectExplorer::IDevice::MachineType);
    QString defaultUserName(ProjectExplorer::IDevice::MachineType);
    QString defaultPassWord(ProjectExplorer::IDevice::MachineType);
    QSize defaultScreenSize(ProjectExplorer::IDevice::MachineType);
    int defaultSSHPort(ProjectExplorer::IDevice::MachineType);

private:
    Q_SLOT void handleAuthTypeChanged();
    Q_SLOT void deviceTypeChanged(int type);
    Internal::GenericSyberDeviceConfigurationWizardSetupPagePrivate * const d;
};

class REMOTELINUX_EXPORT GenericSyberDeviceConfigurationWizardFinalPage : public QWizardPage
{
    Q_OBJECT
public:
    GenericSyberDeviceConfigurationWizardFinalPage(QWidget *parent);
    ~GenericSyberDeviceConfigurationWizardFinalPage();

    void initializePage();

protected:
    virtual QString infoText() const;

private:
    Internal::GenericSyberDeviceConfigurationWizardFinalPagePrivate * const d;
};

} // namespace RemoteLinux

#endif // GENERICSYBERDEVICECONFIGURATIONWIZARDPAGES_H
