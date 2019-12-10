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

#ifndef SYBERDEVICECONFIGURATION_H
#define SYBERDEVICECONFIGURATION_H

#include "../remotelinux_export.h"

#include <projectexplorer/devicesupport/idevice.h>

#include <QCoreApplication>

namespace QSsh { class SshConnectionParameters; }
namespace Utils { class PortList; }

namespace RemoteLinux {
namespace Internal { class LinuxDevicePrivate; }

class REMOTELINUX_EXPORT SyberDevice : public ProjectExplorer::IDevice
{
    Q_DECLARE_TR_FUNCTIONS(RemoteLinux::Internal::SyberDevice)

public:
    typedef QMap<QString, QString> Dict;
    typedef QSharedPointer<SyberDevice> Ptr;
    typedef QSharedPointer<const SyberDevice> ConstPtr;

    static Ptr create();
    static Ptr create(const QString &name, Core::Id type, MachineType machineType,
                      Origin origin = ManuallyAdded, Core::Id id = Core::Id());

    static QStringList availableMachineTypeNames();
    static QStringList availableOSVersions();
    static QString displayNameForMachineType(MachineType);

    QString displayType() const override;
    ProjectExplorer::IDeviceWidget *createWidget() override;
    QList<Core::Id> actionIds() const override;
    QString displayNameForActionId(Core::Id actionId) const override;
    void executeAction(Core::Id actionId, QWidget *parent) override;
    void fromMap(const QVariantMap &map) override;
    QVariantMap toMap() const override;
    ProjectExplorer::IDevice::Ptr clone() const override;

    QString osVersion() const;
    void setOsVersion(const QString&);

    QSize screenSize() const override;
    void setScreenSize(const QSize& size);

    bool canCreateProcess()  const override { return true; }
    ProjectExplorer::DeviceProcess *createProcess(QObject *parent) const override;
    bool canAutoDetectPorts() const override;
    ProjectExplorer::PortsGatheringMethod::Ptr portsGatheringMethod() const override;
    bool canCreateProcessModel() const override { return true; }
    ProjectExplorer::DeviceProcessList *createProcessListModel(QObject *parent) const override;
    bool hasDeviceTester() const override { return true; }
    ProjectExplorer::DeviceTester *createDeviceTester() const override;
    ProjectExplorer::DeviceProcessSignalOperation::Ptr signalOperation() const override;

protected:
    SyberDevice();
    SyberDevice(const QString &name, Core::Id type,
                             MachineType machineType, Origin origin, Core::Id id);
    SyberDevice(const SyberDevice &other);

private:
    Dict m_extraInfo;
    SyberDevice &operator=(const SyberDevice &);
};

} // namespace RemoteLinux

#endif // SYBERDEVICECONFIGURATION_H
