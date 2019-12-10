#include "genericsyberdeviceconfigurationfactory.h"

#include "genericsyberdeviceconfigurationwizard.h"
#include "syberdevice.h"
#include "remotelinux_constants.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <utils/qtcassert.h>

using namespace ProjectExplorer;

namespace RemoteLinux {

GenericSyberDeviceConfigurationFactory::GenericSyberDeviceConfigurationFactory(QObject *parent)
    : IDeviceFactory(parent)
{
}

QString GenericSyberDeviceConfigurationFactory::displayNameForId(Core::Id type) const
{
    QTC_ASSERT(type == ProjectExplorer::Constants::GenericSyberOsType, return QString());
    return tr("Generic Syber Device");
}

QList<Core::Id> GenericSyberDeviceConfigurationFactory::availableCreationIds() const
{
    return QList<Core::Id>() << Core::Id(ProjectExplorer::Constants::GenericSyberOsType);
}

IDevice::Ptr GenericSyberDeviceConfigurationFactory::create(Core::Id id) const
{
    QTC_ASSERT(id == ProjectExplorer::Constants::GenericSyberOsType, return IDevice::Ptr());
    GenericSyberDeviceConfigurationWizard wizard(Core::ICore::mainWindow());
    if (wizard.exec() != QDialog::Accepted)
        return IDevice::Ptr();
    return wizard.device();
}

bool GenericSyberDeviceConfigurationFactory::canRestore(const QVariantMap &map) const
{
    return IDevice::typeFromMap(map) == ProjectExplorer::Constants::GenericSyberOsType;
}

IDevice::Ptr GenericSyberDeviceConfigurationFactory::restore(const QVariantMap &map) const
{
    QTC_ASSERT(canRestore(map), return IDevice::Ptr());
    const IDevice::Ptr device = SyberDevice::create();
    device->fromMap(map);
    return device;
}

} // namespace RemoteLinux
