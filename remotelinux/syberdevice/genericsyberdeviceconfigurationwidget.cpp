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

#include "genericsyberdeviceconfigurationwidget.h"
#include "ui_genericsyberdeviceconfigurationwidget.h"
#include "syberdevice.h"

#include <coreplugin/coreconstants.h>
#include <utils/portlist.h>
#include <utils/fancylineedit.h>
#include <ssh/sshconnection.h>
#include <ssh/sshkeycreationdialog.h>

using namespace ProjectExplorer;
using namespace RemoteLinux;
using namespace QSsh;
using namespace Utils;

GenericSyberDeviceConfigurationWidget::GenericSyberDeviceConfigurationWidget(
        const IDevice::Ptr &deviceConfig, QWidget *parent) :
    IDeviceWidget(deviceConfig, parent),
    m_ui(new Ui::GenericSyberDeviceConfigurationWidget)
{
    m_ui->setupUi(this);
    connect(m_ui->hostLineEdit, SIGNAL(editingFinished()), this, SLOT(hostNameEditingFinished()));
    connect(m_ui->osVersioncomboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(osVersionSelectionChanged()));
    connect(m_ui->userLineEdit, SIGNAL(editingFinished()), this, SLOT(userNameEditingFinished()));
    connect(m_ui->pwdLineEdit, SIGNAL(editingFinished()), this, SLOT(passwordEditingFinished()));
    connect(m_ui->passwordButton, SIGNAL(toggled(bool)), this, SLOT(authenticationTypeChanged()));
    connect(m_ui->keyFileLineEdit, SIGNAL(editingFinished()), this, SLOT(keyFileEditingFinished()));
    connect(m_ui->keyFileLineEdit, SIGNAL(browsingFinished()), this, SLOT(keyFileEditingFinished()));
    connect(m_ui->keyButton, SIGNAL(toggled(bool)), this, SLOT(authenticationTypeChanged()));
    connect(m_ui->timeoutSpinBox, SIGNAL(editingFinished()), this, SLOT(timeoutEditingFinished()));
    connect(m_ui->timeoutSpinBox, SIGNAL(valueChanged(int)), this, SLOT(timeoutEditingFinished()));
    connect(m_ui->sshPortSpinBox, SIGNAL(editingFinished()), this, SLOT(sshPortEditingFinished()));
    connect(m_ui->sshPortSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sshPortEditingFinished()));
    connect(m_ui->showPasswordCheckBox, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));
    connect(m_ui->portsLineEdit, SIGNAL(editingFinished()), this, SLOT(handleFreePortsChanged()));
    connect(m_ui->createKeyButton, SIGNAL(clicked()), SLOT(createNewKey()));
    connect(m_ui->gdbServerLineEdit, SIGNAL(editingFinished()), SLOT(gdbServerEditingFinished()));
    connect(m_ui->hostKeyCheckBox, &QCheckBox::toggled, this,
            &GenericSyberDeviceConfigurationWidget::hostKeyCheckingChanged);

    initGui();
}

GenericSyberDeviceConfigurationWidget::~GenericSyberDeviceConfigurationWidget()
{
    delete m_ui;
}

void GenericSyberDeviceConfigurationWidget::authenticationTypeChanged()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    const bool usePassword = m_ui->passwordButton->isChecked();
    sshParams.authenticationType = usePassword
        ? SshConnectionParameters::AuthenticationTypeTryAllPasswordBasedMethods
        : SshConnectionParameters::AuthenticationTypePublicKey;
    device()->setSshParameters(sshParams);
    m_ui->pwdLineEdit->setEnabled(usePassword);
    m_ui->passwordLabel->setEnabled(usePassword);
    m_ui->keyFileLineEdit->setEnabled(!usePassword);
    m_ui->keyLabel->setEnabled(!usePassword);
}

void GenericSyberDeviceConfigurationWidget::hostNameEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.host = m_ui->hostLineEdit->text().trimmed();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::sshPortEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.port = m_ui->sshPortSpinBox->value();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::timeoutEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.timeout = m_ui->timeoutSpinBox->value();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::userNameEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.userName = m_ui->userLineEdit->text();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::osVersionSelectionChanged()
{
    auto dev = qSharedPointerCast<SyberDevice>(device());
    dev->setOsVersion(m_ui->osVersioncomboBox->currentText());
}

void GenericSyberDeviceConfigurationWidget::passwordEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.password = m_ui->pwdLineEdit->text();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::keyFileEditingFinished()
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.privateKeyFile = m_ui->keyFileLineEdit->path();
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::gdbServerEditingFinished()
{
    device()->setDebugServerPath(m_ui->gdbServerLineEdit->text());
}

void GenericSyberDeviceConfigurationWidget::handleFreePortsChanged()
{
    device()->setFreePorts(PortList::fromString(m_ui->portsLineEdit->text()));
    updatePortsWarningLabel();
}

void GenericSyberDeviceConfigurationWidget::showPassword(bool showClearText)
{
    m_ui->pwdLineEdit->setEchoMode(showClearText
        ? QLineEdit::Normal : QLineEdit::Password);
}

void GenericSyberDeviceConfigurationWidget::setPrivateKey(const QString &path)
{
    m_ui->keyFileLineEdit->setPath(path);
    keyFileEditingFinished();
}

void GenericSyberDeviceConfigurationWidget::createNewKey()
{
    SshKeyCreationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        setPrivateKey(dialog.privateKeyFilePath());
}

void GenericSyberDeviceConfigurationWidget::hostKeyCheckingChanged(bool doCheck)
{
    SshConnectionParameters sshParams = device()->sshParameters();
    sshParams.hostKeyCheckingMode
            = doCheck ? SshHostKeyCheckingAllowNoMatch : SshHostKeyCheckingNone;
    device()->setSshParameters(sshParams);
}

void GenericSyberDeviceConfigurationWidget::updateDeviceFromUi()
{
    hostNameEditingFinished();
    sshPortEditingFinished();
    timeoutEditingFinished();
    userNameEditingFinished();
    osVersionSelectionChanged();
    passwordEditingFinished();
    keyFileEditingFinished();
    handleFreePortsChanged();
    gdbServerEditingFinished();
}

void GenericSyberDeviceConfigurationWidget::updatePortsWarningLabel()
{
    m_ui->portsWarningLabel->setVisible(!device()->freePorts().hasMore());
}

void GenericSyberDeviceConfigurationWidget::initGui()
{
    m_ui->machineTypeComobox->clear();
//    QStringList mtList = SyberDevice::availableMachineTypes();
//    foreach (const QString& str, mtList) {
//            m_ui->machineTypeComobox->addItem(
//                        QCoreApplication::translate("RemoteLinux::SyberDevice", str.toLocal8Bit().constData(), 0));
//    }
    m_ui->machineTypeComobox->addItems(SyberDevice::availableMachineTypeNames());
    m_ui->machineTypeComobox->setCurrentIndex((int)device()->machineType());
    auto dev =qSharedPointerCast<SyberDevice> (device());
    QString version = dev->osVersion();
    m_ui->osVersioncomboBox->clear();
    m_ui->osVersioncomboBox->addItems(SyberDevice::availableOSVersions());
    m_ui->osVersioncomboBox->setCurrentText(version);

//    m_ui->portsWarningLabel->setPixmap(QPixmap(QLatin1String(Core::Constants::ICON_ERROR)));
    m_ui->portsWarningLabel->setToolTip(QLatin1String("<font color=\"red\">")
        + tr("You will need at least one port.") + QLatin1String("</font>"));
    m_ui->keyFileLineEdit->setExpectedKind(PathChooser::File);
    m_ui->keyFileLineEdit->setHistoryCompleter(QLatin1String("Ssh.KeyFile.History"));
    m_ui->keyFileLineEdit->lineEdit()->setMinimumWidth(0);
    QRegExpValidator * const portsValidator
        = new QRegExpValidator(QRegExp(PortList::regularExpression()), this);
    m_ui->portsLineEdit->setValidator(portsValidator);

    const SshConnectionParameters &sshParams = device()->sshParameters();

    if (sshParams.authenticationType != SshConnectionParameters::AuthenticationTypePublicKey)
        m_ui->passwordButton->setChecked(true);
    else
        m_ui->keyButton->setChecked(true);
    m_ui->timeoutSpinBox->setValue(sshParams.timeout);
    m_ui->hostLineEdit->setEnabled(!device()->isAutoDetected());
    m_ui->sshPortSpinBox->setEnabled(!device()->isAutoDetected());
    m_ui->hostKeyCheckBox->setChecked(sshParams.hostKeyCheckingMode != SshHostKeyCheckingNone);

    m_ui->hostLineEdit->setText(sshParams.host);
    m_ui->sshPortSpinBox->setValue(sshParams.port);
    m_ui->portsLineEdit->setText(device()->freePorts().toString());
    m_ui->timeoutSpinBox->setValue(sshParams.timeout);
    m_ui->userLineEdit->setText(sshParams.userName);
    m_ui->pwdLineEdit->setText(sshParams.password);
    m_ui->keyFileLineEdit->setPath(sshParams.privateKeyFile);
    m_ui->showPasswordCheckBox->setChecked(false);
    m_ui->gdbServerLineEdit->setText(device()->debugServerPath());
    updatePortsWarningLabel();
}
