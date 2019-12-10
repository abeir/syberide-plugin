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

#include "remotesyberruncontrol.h"

#include "abstractremotelinuxrunconfiguration.h"

#include <projectexplorer/devicesupport/deviceapplicationrunner.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectinformation.h>
#include <utils/environment.h>

#include <QString>
#include <QIcon>
#include <QMessageBox>

using namespace ProjectExplorer;

namespace RemoteLinux {

class RemoteSyberRunControl::RemoteSyberRunControlPrivate
{
public:
    bool running;
    DeviceApplicationRunner runner;
    IDevice::ConstPtr device;
    QString remoteExecutable;
    QStringList arguments;
    Utils::Environment environment;
    QString workingDir;
};

RemoteSyberRunControl::RemoteSyberRunControl(RunConfiguration *rc)
        : RunControl(rc, ProjectExplorer::Constants::NORMAL_RUN_MODE), d(new RemoteSyberRunControlPrivate)
{
    setIcon(QLatin1String(ProjectExplorer::Constants::ICON_RUN_SMALL));

    d->running = false;
    d->device = DeviceKitInformation::device(rc->target()->kit());
    const AbstractRemoteLinuxRunConfiguration * const lrc = qobject_cast<AbstractRemoteLinuxRunConfiguration *>(rc);
    d->remoteExecutable = lrc->remoteExecutableFilePath();
    d->arguments = lrc->arguments();
    d->environment = lrc->environment();
    d->workingDir = lrc->workingDirectory();
}

RemoteSyberRunControl::~RemoteSyberRunControl()
{
    delete d;
}
#include <QDebug>
void RemoteSyberRunControl::start()
{
    d->running = true;
    emit started();
    d->runner.disconnect(this);
    connect(&d->runner, SIGNAL(reportError(QString)), SLOT(handleErrorMessage(QString)));
    connect(&d->runner, SIGNAL(remoteStderr(QByteArray)),
        SLOT(handleRemoteErrorOutput(QByteArray)));
    connect(&d->runner, SIGNAL(remoteStdout(QByteArray)), SLOT(handleRemoteOutput(QByteArray)));
    connect(&d->runner, SIGNAL(finished(bool)), SLOT(handleRunnerFinished()));
    connect(&d->runner, SIGNAL(reportProgress(QString)), SLOT(handleProgressReport(QString)));
    d->runner.setEnvironment(d->environment);
    d->runner.setWorkingDirectory(d->workingDir);

    if(project()->isSyberProject())
    {
        QString strCmd;
        QString sopId = ProjectInformation::instance().curProjectSopid();
        QString ExecType = ProjectExplorer::ProjectInformation::instance().curProjectRunExecInfo(d->remoteExecutable);
        if(!ExecType.isEmpty())
        {
            strCmd = QLatin1String("/usr/bin/sdk-invoker 0 ");
            strCmd += sopId + QLatin1String(":") + ExecType;
        }
        else
        {
            strCmd = QLatin1String("/usr/bin/run-as ");
            strCmd += sopId + QString::fromLatin1(" ") + d->remoteExecutable;
        }
        qDebug() << "[RemoteSyberRunControl]: strCmd =  " << strCmd;
        qDebug() << "[RemoteSyberRunControl]: Aruguments =  " << d->arguments;
        d->runner.start(d->device, strCmd, d->arguments);
        d->runner.setExecutableFile(d->remoteExecutable);
    }
    else
        d->runner.start(d->device, d->remoteExecutable, d->arguments);
}

RunControl::StopResult RemoteSyberRunControl::stop()
{
    d->runner.stop();
    return AsynchronousStop;
}

void RemoteSyberRunControl::handleErrorMessage(const QString &error)
{
    appendMessage(error, Utils::ErrorMessageFormat);
}

void RemoteSyberRunControl::handleRunnerFinished()
{
    setFinished();
}

void RemoteSyberRunControl::handleRemoteOutput(const QByteArray &output)
{
    appendMessage(QString::fromUtf8(output), Utils::StdOutFormatSameLine);
}

void RemoteSyberRunControl::handleRemoteErrorOutput(const QByteArray &output)
{
    appendMessage(QString::fromUtf8(output), Utils::StdErrFormatSameLine);
}

void RemoteSyberRunControl::handleProgressReport(const QString &progressString)
{
    appendMessage(progressString + QLatin1Char('\n'), Utils::NormalMessageFormat);
}

bool RemoteSyberRunControl::isRunning() const
{
    return d->running;
}

void RemoteSyberRunControl::setFinished()
{
    d->runner.disconnect(this);
    d->running = false;
    emit finished();
}

} // namespace RemoteLinux
