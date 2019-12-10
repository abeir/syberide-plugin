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
#include "syberdeviceprocess.h"
#include <projectexplorer/runnables.h>

#include <utils/environment.h>
#include <utils/qtcprocess.h>

#include <QDebug>
using namespace ProjectExplorer;
using namespace Utils;

namespace RemoteLinux {

static QString quote(const QString &s) { return Utils::QtcProcess::quoteArgUnix(s); }

SyberDeviceProcess::SyberDeviceProcess(const QSharedPointer<const ProjectExplorer::IDevice> &device,
        QObject *parent)
    : ProjectExplorer::SshDeviceProcess(device, parent)
{
}

void SyberDeviceProcess::setRcFilesToSource(const QStringList &filePaths)
{
    m_rcFilesToSource = filePaths;
}

QString SyberDeviceProcess::fullCommandLine(const StandardRunnable &runnable) const
{
    const Environment env = runnable.environment;

    QString fullCommandLine;
    foreach (const QString &filePath, rcFilesToSource())
        fullCommandLine += QString::fromLatin1("test -f %1 && . %1;").arg(filePath);

    // Will add this at 20150122 for compositor need sleep after env set. THis is a bug.
    //fullCommandLine += QString::fromLatin1("sleep 0.8;");


    if (!runnable.workingDirectory.isEmpty()) {
        fullCommandLine.append(QLatin1String("cd ")).append(quote(runnable.workingDirectory))
                .append(QLatin1String(" && "));
    }
    const QString envString = env.toStringList().join(QLatin1String(" "));
    if (!envString.isEmpty())
        fullCommandLine.append(QLatin1Char(' ')).append(envString);
    if (!fullCommandLine.isEmpty())
        fullCommandLine += QLatin1Char(' ');

    fullCommandLine.append(runnable.executable);

    if (!runnable.commandLineArguments.isEmpty()) {
        fullCommandLine.append(QLatin1Char(' '));
        fullCommandLine.append(runnable.commandLineArguments);
    }

    qDebug() << "[SyberDebug]: SyberDeviceProcess.cpp <fullCommandLine> fullCommandLine =  " << fullCommandLine;
    return fullCommandLine;
}

QStringList SyberDeviceProcess::rcFilesToSource() const
{
    if (!m_rcFilesToSource.isEmpty())
        return m_rcFilesToSource;
    return QStringList() << QLatin1String("/etc/profile") << QLatin1String("$HOME/.profile");
}

} // namespace RemoteLinux
