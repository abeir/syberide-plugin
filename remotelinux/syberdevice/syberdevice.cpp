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

#include "syberdevice.h"

#include "genericsyberdeviceconfigurationwidget.h"
#include "syberdeviceprocess.h"
#include "linuxdevicetester.h"
#include "publickeydeploymentdialog.h"
#include "remotelinux_constants.h"
#include "remotesybersignaloperation.h"

#include <coreplugin/id.h>
#include <projectexplorer/devicesupport/sshdeviceprocesslist.h>
#include <ssh/sshremoteprocessrunner.h>
#include <utils/algorithm.h>
#include <utils/qtcassert.h>

#include <QTimer>

using namespace ProjectExplorer;

namespace RemoteLinux {

const char Delimiter0[] = "x--";
const char Delimiter1[] = "---";
const char EXTRA_INFO_KEY[] = "extraInfo";
const char DEFAULT_OS_VERSION[] = "2.0";
const char OS_VERSION_KEY[] = "osVersion";
const char SCREEN_WIDTH_KEY[] = "screenWidth";
const char SCREEN_HEIGHT_KEY[] = "screenHeight";

const QSize defaultScreenSize = QSize(1280, 720);

static QString visualizeNull(QString s)
{
    return s.replace(QLatin1Char('\0'), QLatin1String("<null>"));
}

class SyberDeviceProcessList : public SshDeviceProcessList
{
public:
    SyberDeviceProcessList(const IDevice::ConstPtr &device, QObject *parent)
            : SshDeviceProcessList(device, parent)
    {
    }

private:
    QString listProcessesCommandLine() const
    {
        return QString::fromLatin1(
            "for dir in `ls -d /proc/[0123456789]*`; do "
                "test -d $dir || continue;" // Decrease the likelihood of a race condition.
                "echo $dir;"
                "cat $dir/cmdline;echo;" // cmdline does not end in newline
                "cat $dir/stat;"
                "readlink $dir/exe;"
                "printf '%1''%2';"
            "done").arg(QLatin1String(Delimiter0)).arg(QLatin1String(Delimiter1));
    }

    QList<DeviceProcessItem> buildProcessList(const QString &listProcessesReply) const
    {
        QList<DeviceProcessItem> processes;
        const QStringList lines = listProcessesReply.split(QString::fromLatin1(Delimiter0)
                + QString::fromLatin1(Delimiter1), QString::SkipEmptyParts);
        foreach (const QString &line, lines) {
            const QStringList elements = line.split(QLatin1Char('\n'));
            if (elements.count() < 4) {
                qDebug("%s: Expected four list elements, got %d. Line was '%s'.", Q_FUNC_INFO,
                       elements.count(), qPrintable(visualizeNull(line)));
                continue;
            }
            bool ok;
            const int pid = elements.first().mid(6).toInt(&ok);
            if (!ok) {
                qDebug("%s: Expected number in %s. Line was '%s'.", Q_FUNC_INFO,
                       qPrintable(elements.first()), qPrintable(visualizeNull(line)));
                continue;
            }
            QString command = elements.at(1);
            command.replace(QLatin1Char('\0'), QLatin1Char(' '));
            if (command.isEmpty()) {
                const QString &statString = elements.at(2);
                const int openParenPos = statString.indexOf(QLatin1Char('('));
                const int closedParenPos = statString.indexOf(QLatin1Char(')'), openParenPos);
                if (openParenPos == -1 || closedParenPos == -1)
                    continue;
                command = QLatin1Char('[')
                        + statString.mid(openParenPos + 1, closedParenPos - openParenPos - 1)
                        + QLatin1Char(']');
            }

            DeviceProcessItem process;
            process.pid = pid;
            process.cmdLine = command;
            process.exe = elements.at(3);
            processes.append(process);
        }

        Utils::sort(processes);
        return processes;
    }
};

class SyberPortsGatheringMethod : public PortsGatheringMethod
{
    QByteArray commandLine(QAbstractSocket::NetworkLayerProtocol protocol) const
    {
        QString procFilePath;
        int addressLength;
        if (protocol == QAbstractSocket::IPv4Protocol) {
            procFilePath = QLatin1String("/proc/net/tcp");
            addressLength = 8;
        } else {
            procFilePath = QLatin1String("/proc/net/tcp6");
            addressLength = 32;
        }
        return QString::fromLatin1("sed "
                "'s/.*: [[:xdigit:]]\\{%1\\}:\\([[:xdigit:]]\\{4\\}\\).*/\\1/g' %2")
                .arg(addressLength).arg(procFilePath).toUtf8();
    }

    QList<int> usedPorts(const QByteArray &output) const
    {
        QList<int> ports;
        QList<QByteArray> portStrings = output.split('\n');
        portStrings.removeFirst();
        foreach (const QByteArray &portString, portStrings) {
            if (portString.isEmpty())
                continue;
            bool ok;
            const int port = portString.toInt(&ok, 16);
            if (ok) {
                if (!ports.contains(port))
                    ports << port;
            } else {
                qWarning("%s: Unexpected string '%s' is not a port.",
                         Q_FUNC_INFO, portString.data());
            }
        }
        return ports;
    }
};


SyberDevice::Ptr SyberDevice::create(const QString &name,
       Core::Id type, MachineType machineType, Origin origin, Core::Id id)
{
    return Ptr(new SyberDevice(name, type, machineType, origin, id));
}


QStringList SyberDevice::availableMachineTypeNames()
{
    QStringList list;
    list.append(displayNameForMachineType(IDevice::Hardware));
    list.append(displayNameForMachineType(IDevice::Emulator));
    return list;
}

QStringList SyberDevice::availableOSVersions()
{
    QStringList list;
    list.append(tr("2.0"));
    list.append(tr("2.1"));
    return list;
}

QString SyberDevice::displayNameForMachineType(MachineType type)
{
    if (type == IDevice::Hardware)
        return QCoreApplication::translate("RemoteLinux::SyberDevice", "Physical Device");
    return QCoreApplication::translate("RemoteLinux::SyberDevice", "Emulator");
}

QString SyberDevice::displayType() const
{
    return QCoreApplication::translate("RemoteLinux::SyberDevice", "Generic SyberOS");
}

IDeviceWidget *SyberDevice::createWidget()
{
    return new GenericSyberDeviceConfigurationWidget(sharedFromThis());
}

QList<Core::Id> SyberDevice::actionIds() const
{
    return QList<Core::Id>() << Core::Id(Constants::GenericDeployKeyToDeviceActionId);
}

QString SyberDevice::displayNameForActionId(Core::Id actionId) const
{
    QTC_ASSERT(actionIds().contains(actionId), return QString());

    if (actionId == Constants::GenericDeployKeyToDeviceActionId)
        return QCoreApplication::translate("RemoteLinux::SyberDevice", "Deploy Public Key...");
    return QString(); // Can't happen.
}

void SyberDevice::executeAction(Core::Id actionId, QWidget *parent)
{
    QTC_ASSERT(actionIds().contains(actionId), return);

    QDialog *d = 0;
    const SyberDevice::ConstPtr device = sharedFromThis().staticCast<const SyberDevice>();
    if (actionId == Constants::GenericDeployKeyToDeviceActionId)
        d = PublicKeyDeploymentDialog::createDialog(device, parent);
    if (d)
        d->exec();
    delete d;
}

SyberDevice::SyberDevice()
{
    setOsVersion(QLatin1String(DEFAULT_OS_VERSION));
    setScreenSize(defaultScreenSize);
}

SyberDevice::SyberDevice(const QString &name, Core::Id type, MachineType machineType,
        Origin origin, Core::Id id)
    : IDevice(type, origin, machineType, id)
{
    setDisplayName(name);
    setOsVersion(QLatin1String(DEFAULT_OS_VERSION));
    setScreenSize(defaultScreenSize);
}

SyberDevice::SyberDevice(const SyberDevice &other)
    : IDevice(other), m_extraInfo(other.m_extraInfo)
{
}

SyberDevice::Ptr SyberDevice::create()
{
    return Ptr(new SyberDevice);
}

void SyberDevice::fromMap(const QVariantMap &map)
{
    IDevice::fromMap(map);
    QVariantMap vMap = map.value(QLatin1String(EXTRA_INFO_KEY)).toMap();
    QMapIterator<QString, QVariant> i(vMap);
    m_extraInfo.clear();
    while (i.hasNext()) {
        i.next();
        m_extraInfo.insert(i.key(), i.value().toString());
    }
}

QVariantMap SyberDevice::toMap() const
{
    QVariantMap res = IDevice::toMap();
    QVariantMap vMap;
    QMapIterator<QString, QString> i(m_extraInfo);
    while (i.hasNext()) {
        i.next();
        vMap.insert(i.key(), i.value());
    }
    res.insert(QLatin1String(EXTRA_INFO_KEY), vMap);
    return res;
}

IDevice::Ptr SyberDevice::clone() const
{
    return Ptr(new SyberDevice(*this));
}

QString SyberDevice::osVersion() const
{
    return m_extraInfo.value(QLatin1String(OS_VERSION_KEY));
}

void SyberDevice::setOsVersion(const QString& version)
{
    if(m_extraInfo.contains(QLatin1String(OS_VERSION_KEY)))
        m_extraInfo.remove(QLatin1String(OS_VERSION_KEY));
    m_extraInfo.insert(QLatin1String(OS_VERSION_KEY), version);
}

QSize SyberDevice::screenSize() const
{
    QSize size = defaultScreenSize;
    if(m_extraInfo.contains(QLatin1String(SCREEN_HEIGHT_KEY))
            && m_extraInfo.contains(QLatin1String(SCREEN_WIDTH_KEY)))
    {
        int height = m_extraInfo.value(QLatin1String(SCREEN_HEIGHT_KEY)).toInt();
        int width = m_extraInfo.value(QLatin1String(SCREEN_WIDTH_KEY)).toInt();
        size.setHeight(height);
        size.setWidth(width);
    }
    return size;
}

void SyberDevice::setScreenSize(const QSize& size)
{
    if(m_extraInfo.contains(QLatin1String(SCREEN_HEIGHT_KEY))
            || m_extraInfo.contains(QLatin1String(SCREEN_WIDTH_KEY)))
    {
        m_extraInfo.remove(QLatin1String(SCREEN_HEIGHT_KEY));
        m_extraInfo.remove(QLatin1String(SCREEN_WIDTH_KEY));
    }
    m_extraInfo.insert(QLatin1String(SCREEN_WIDTH_KEY), QString::number(size.width()));
    m_extraInfo.insert(QLatin1String(SCREEN_HEIGHT_KEY), QString::number(size.height()));
}

DeviceProcess *SyberDevice::createProcess(QObject *parent) const
{
    return new SyberDeviceProcess(sharedFromThis(), parent);
}

bool SyberDevice::canAutoDetectPorts() const
{
    return true;
}

PortsGatheringMethod::Ptr SyberDevice::portsGatheringMethod() const
{
    return SyberPortsGatheringMethod::Ptr(new SyberPortsGatheringMethod);
}

DeviceProcessList *SyberDevice::createProcessListModel(QObject *parent) const
{
    return new SyberDeviceProcessList(sharedFromThis(), parent);
}

DeviceTester *SyberDevice::createDeviceTester() const
{
    return new GenericLinuxDeviceTester;
}

DeviceProcessSignalOperation::Ptr SyberDevice::signalOperation() const
{
    return DeviceProcessSignalOperation::Ptr(new RemoteSyberSignalOperation(sshParameters()));
}

} // namespace RemoteLinux
