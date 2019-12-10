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

#include "genericsyberdeviceconfigurationwizardpages.h"
#include "ui_genericsyberdeviceconfigurationwizardsetuppage.h"
#include "syberdevice.h"
#include <projectexplorer/devicesupport/idevice.h>

namespace RemoteLinux {
namespace Internal {

class GenericSyberDeviceConfigurationWizardSetupPagePrivate
{
public:
    Ui::GenericSyberDeviceConfigurationWizardSetupPage ui;

};

class GenericSyberDeviceConfigurationWizardFinalPagePrivate
{
public:
    QLabel infoLabel;
};

} // namespace Internal

using namespace QSsh;
using namespace Utils;

GenericSyberDeviceConfigurationWizardSetupPage::GenericSyberDeviceConfigurationWizardSetupPage(QWidget *parent) :
    QWizardPage(parent), d(new Internal::GenericSyberDeviceConfigurationWizardSetupPagePrivate)
{
    d->ui.setupUi(this);
    setTitle(tr("Connection"));
    setSubTitle(QLatin1String(" ")); // For Qt bug (background color)
    d->ui.privateKeyPathChooser->setExpectedKind(PathChooser::File);
    d->ui.privateKeyPathChooser->setHistoryCompleter(QLatin1String("Ssh.KeyFile.History"));
    d->ui.privateKeyPathChooser->setPromptDialogTitle(tr("Choose a Private Key File"));
    QStringList typeList = SyberDevice::availableMachineTypeNames();
    d->ui.deviceTypeComBox->addItems(typeList);
    QStringList osList = SyberDevice::availableOSVersions();
    d->ui.osVersioncomboBox->addItems(osList);
    connect(d->ui.nameLineEdit, SIGNAL(textChanged(QString)), SIGNAL(completeChanged()));
    connect(d->ui.hostNameLineEdit, SIGNAL(textChanged(QString)), SIGNAL(completeChanged()));
    connect(d->ui.userNameLineEdit, SIGNAL(textChanged(QString)), SIGNAL(completeChanged()));
    connect(d->ui.privateKeyPathChooser, SIGNAL(validChanged(bool)), SIGNAL(completeChanged()));
    connect(d->ui.spinBox_ssh_port,SIGNAL(valueChanged(int)),SIGNAL(completeChanged()));
    connect(d->ui.osVersioncomboBox,SIGNAL(activated(int)),SIGNAL(completeChanged()));
    connect(d->ui.passwordButton, SIGNAL(toggled(bool)), SLOT(handleAuthTypeChanged()));
    connect(d->ui.deviceTypeComBox, SIGNAL(currentIndexChanged(int)), SLOT(deviceTypeChanged(int)));

}

void GenericSyberDeviceConfigurationWizardSetupPage::deviceTypeChanged(int type)
{
    ProjectExplorer::IDevice::MachineType machineType = (ProjectExplorer::IDevice::MachineType) type;
    d->ui.nameLineEdit->setText(defaultConfigurationName(machineType));
    d->ui.hostNameLineEdit->setText(defaultHostName(machineType));
    d->ui.userNameLineEdit->setText(defaultUserName(machineType));
    d->ui.passwordLineEdit->setText(defaultPassWord(machineType));
    d->ui.spinBox_ssh_port->setValue(defaultSSHPort(machineType));
}

GenericSyberDeviceConfigurationWizardSetupPage::~GenericSyberDeviceConfigurationWizardSetupPage()
{
    delete d;
}

void GenericSyberDeviceConfigurationWizardSetupPage::initializePage()
{
    ProjectExplorer::IDevice::MachineType machineType = (ProjectExplorer::IDevice::MachineType )
            d->ui.deviceTypeComBox->currentIndex();
    deviceTypeChanged(machineType);
    d->ui.passwordButton->setChecked(true);
    d->ui.privateKeyPathChooser->setPath(ProjectExplorer::IDevice::defaultPrivateKeyFilePath());
    handleAuthTypeChanged();
}

bool GenericSyberDeviceConfigurationWizardSetupPage::isComplete() const
{
    return !configurationName().isEmpty() && !hostName().isEmpty() && !userName().isEmpty()
            && (authenticationType() != SshConnectionParameters::AuthenticationTypePublicKey
                || d->ui.privateKeyPathChooser->isValid());
}

int GenericSyberDeviceConfigurationWizardSetupPage::sshPort() const
{
    return d->ui.spinBox_ssh_port->value();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::configurationName() const
{
    return d->ui.nameLineEdit->text().trimmed();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::hostName() const
{
    return d->ui.hostNameLineEdit->text().trimmed();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::userName() const
{
    return d->ui.userNameLineEdit->text().trimmed();
}

SshConnectionParameters::AuthenticationType GenericSyberDeviceConfigurationWizardSetupPage::authenticationType() const
{
    return d->ui.passwordButton->isChecked()
        ? SshConnectionParameters::AuthenticationTypeTryAllPasswordBasedMethods
        : SshConnectionParameters::AuthenticationTypePublicKey;
}

QString GenericSyberDeviceConfigurationWizardSetupPage::password() const
{
    return d->ui.passwordLineEdit->text();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::privateKeyFilePath() const
{
    return d->ui.privateKeyPathChooser->path();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::osVersion() const
{
    return d->ui.osVersioncomboBox->currentText();
}

QSize GenericSyberDeviceConfigurationWizardSetupPage::screenSize() const
{
    return QSize(1280, 720);
}

ProjectExplorer::IDevice::MachineType GenericSyberDeviceConfigurationWizardSetupPage::machineType() const
{
    return (ProjectExplorer::IDevice::MachineType) d->ui.deviceTypeComBox->currentIndex();
}

QString GenericSyberDeviceConfigurationWizardSetupPage::defaultConfigurationName(
        ProjectExplorer::IDevice::MachineType type)
{
    if(type == ProjectExplorer::IDevice::Hardware)
        return QLatin1String("Device");
    else
        return QLatin1String("Simulator");
}

QString GenericSyberDeviceConfigurationWizardSetupPage::defaultHostName(
        ProjectExplorer::IDevice::MachineType type)
{
    if(type == ProjectExplorer::IDevice::Hardware)
        return QLatin1String("192.168.100.100");
    else
        return QLatin1String("localhost");
}

QString GenericSyberDeviceConfigurationWizardSetupPage::defaultUserName(
        ProjectExplorer::IDevice::MachineType)
{
    return QLatin1String("developer");
}

QString GenericSyberDeviceConfigurationWizardSetupPage::defaultPassWord(
        ProjectExplorer::IDevice::MachineType)
{
    return QLatin1String("system");
}

int GenericSyberDeviceConfigurationWizardSetupPage::defaultSSHPort(ProjectExplorer::IDevice::MachineType type)
{
    if(type == ProjectExplorer::IDevice::Emulator)
        return 5555;
    else
        return 22;
}

void GenericSyberDeviceConfigurationWizardSetupPage::handleAuthTypeChanged()
{
    d->ui.passwordLineEdit->setEnabled(authenticationType() != SshConnectionParameters::AuthenticationTypePublicKey);
    d->ui.privateKeyPathChooser->setEnabled(!d->ui.passwordLineEdit->isEnabled());
    emit completeChanged();
}

GenericSyberDeviceConfigurationWizardFinalPage::GenericSyberDeviceConfigurationWizardFinalPage(QWidget *parent)
    : QWizardPage(parent), d(new Internal::GenericSyberDeviceConfigurationWizardFinalPagePrivate)
{
    setTitle(tr("Summary"));
    setSubTitle(QLatin1String(" ")); // For Qt bug (background color)
    d->infoLabel.setWordWrap(true);
    QVBoxLayout * const layout = new QVBoxLayout(this);
    layout->addWidget(&d->infoLabel);
}

GenericSyberDeviceConfigurationWizardFinalPage::~GenericSyberDeviceConfigurationWizardFinalPage()
{
    delete d;
}

void GenericSyberDeviceConfigurationWizardFinalPage::initializePage()
{
    d->infoLabel.setText(infoText());
}

QString GenericSyberDeviceConfigurationWizardFinalPage::infoText() const
{
    return tr("The new device configuration will now be created.\n"
        "In addition, device connectivity will be tested.");
}

} // namespace RemoteLinux
