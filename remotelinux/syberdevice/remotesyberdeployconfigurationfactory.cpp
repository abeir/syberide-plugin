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
#include "remotesyberdeployconfigurationfactory.h"

#include "genericdirectuploadstep.h"
#include "remotelinuxcheckforfreediskspacestep.h"
#include "remotelinux_constants.h"
#include "remotelinuxdeployconfiguration.h"
#include "remotelinuxcustomcommanddeploymentstep.h"
#include "startvmstep.h"
#include "initsshconnectstep.h"
#include "remotesyberinstallpackagestep.h"

#include <projectexplorer/abi.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildconfiguration.h>
#include <qmakeprojectmanager/buildpkgstep.h>
#include <ssh/sshconnection.h>
#include <qtsupport/qtkitinformation.h>

#include <QCoreApplication>
#include <QDebug>

using namespace ProjectExplorer;

namespace RemoteLinux {
namespace Internal {
namespace {
QString genericSyberDisplayName() {
    return QCoreApplication::translate("RemoteSyber", "Deploy to Remote Syber Host");
}
} // anonymous namespace

RemoteSyberDeployConfigurationFactory::RemoteSyberDeployConfigurationFactory(QObject *parent)
    : DeployConfigurationFactory(parent)
{ setObjectName(QLatin1String("RemoteSyberDeployConfiguration"));}

QList<Core::Id> RemoteSyberDeployConfigurationFactory::availableCreationIds(Target *target) const
{
    QList<Core::Id> ids;
    if (!target->project()->supportsKit(target->kit()))
        return ids;

    const Core::Id deviceType = DeviceTypeKitInformation::deviceTypeId(target->kit());
    if (deviceType != ProjectExplorer::Constants::GenericSyberOsType)
        return ids;

    ids << genericDeployConfigurationId();
    return ids;
}

QString RemoteSyberDeployConfigurationFactory::displayNameForId(Core::Id id) const
{
    if (id == genericDeployConfigurationId())
        return genericSyberDisplayName();
    return QString();
}

bool RemoteSyberDeployConfigurationFactory::canCreate(Target *parent, Core::Id id) const
{
    return availableCreationIds(parent).contains(id);
}

DeployConfiguration *RemoteSyberDeployConfigurationFactory::create(Target *parent,
    const Core::Id id)
{
    Q_ASSERT(canCreate(parent, id));

    RemoteLinuxDeployConfiguration * const dc = new RemoteLinuxDeployConfiguration(parent, id,
        genericSyberDisplayName());

    // Modify by fh 2015/6/26
#ifdef ENABLE_QT_DEPLOY		/*Added by yipan.chen*/
    int n = 0;

    if(dc->isBuildPkgNeeded())
    {
        dc->stepList()->insertStep(n++, new QmakeProjectManager::BuildpkgStep(dc->stepList()));
    }

    if(dc->isStartVMNeeded())
    {
        StartVMStep *startVM = new StartVMStep(dc->stepList());
        QtSupport::BaseQtVersion *qtVersion = QtSupport::QtKitInformation::qtVersion(parent->kit());
        if (qtVersion){
            QString osVersion = qtVersion->qmakeProperty("SYBEROS_OSVERSION");
            startVM->setOsArguments(osVersion);
            qDebug()<<"[Syberdebug]: RemoteSyberDeployConfigurationFactory-- osVersion  is "<< osVersion<<endl;
        }
        dc->stepList()->insertStep(n++, startVM);
    }

//    InitSshConnectStep* ds = new InitSshConnectStep(dc->stepList());
//    dc->stepList()->insertStep(n++, ds);

    //dc->stepList()->insertStep(n++, new RemoteLinuxCheckForFreeDiskSpaceStep(dc->stepList()));

    GenericDirectUploadStep* du = new GenericDirectUploadStep(dc->stepList(),
                                                              GenericDirectUploadStep::stepId());
    du->setIgnoreChmod(true);
    dc->stepList()->insertStep(n++, du );

    if(dc->isBuildPkgNeeded())
    {
        dc->stepList()->insertStep(n++, new RemoteSyberInstallPackageStep(dc->stepList()));
    }
#endif
    return dc;
}

bool RemoteSyberDeployConfigurationFactory::canRestore(Target *parent, const QVariantMap &map) const
{
    return canCreate(parent, idFromMap(map));
}

DeployConfiguration *RemoteSyberDeployConfigurationFactory::restore(Target *parent,
    const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    Core::Id id = idFromMap(map);
    RemoteLinuxDeployConfiguration * const dc = new RemoteLinuxDeployConfiguration(parent, id,
        genericSyberDisplayName());
    if (!dc->fromMap(map)) {
        delete dc;
        return 0;
    }
    return dc;
}

bool RemoteSyberDeployConfigurationFactory::canClone(Target *parent, DeployConfiguration *product) const
{
    return canCreate(parent, product->id());
}

DeployConfiguration *RemoteSyberDeployConfigurationFactory::clone(Target *parent,
    DeployConfiguration *product)
{
    if (!canClone(parent, product))
        return 0;
    return new RemoteLinuxDeployConfiguration(parent,
        qobject_cast<RemoteLinuxDeployConfiguration *>(product));
}

Core::Id RemoteSyberDeployConfigurationFactory::genericDeployConfigurationId()
{
    return "DeployToGenericSyber";
}

} // namespace Internal
} // namespace RemoteLinux
