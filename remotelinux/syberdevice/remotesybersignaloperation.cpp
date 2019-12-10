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

#include "remotesybersignaloperation.h"

#include <ssh/sshremoteprocessrunner.h>
#include <ssh/sshconnection.h>
#include <utils/qtcassert.h>
#include <QRegExp>

#include <projectexplorer/projectinformation.h>

using namespace RemoteLinux;
using namespace ProjectExplorer;

RemoteSyberSignalOperation::RemoteSyberSignalOperation(
        const QSsh::SshConnectionParameters &sshParameters)
    : DeviceProcessSignalOperation()
    , m_sshParameters(sshParameters)
    , m_runner(0)
{}

RemoteSyberSignalOperation::~RemoteSyberSignalOperation()
{
    if (m_runner) {
        connect(m_runner, &QSsh::SshRemoteProcessRunner::processClosed,
                m_runner, &QSsh::SshRemoteProcessRunner::deleteLater);
        connect(m_runner, &QSsh::SshRemoteProcessRunner::connectionError,
                m_runner, &QSsh::SshRemoteProcessRunner::deleteLater);
    }
}

static QString signalProcessByPidCommandLine(qint64 pid, int signal)
{
    return QString::fromLatin1("kill -%1 %2").arg(signal).arg(pid);
}

void RemoteSyberSignalOperation::run(const QString &command)
{
    QTC_ASSERT(!m_runner, return);
    m_runner = new QSsh::SshRemoteProcessRunner();
    connect(m_runner, SIGNAL(processClosed(int)), SLOT(runnerProcessFinished()));
    connect(m_runner, SIGNAL(connectionError()), SLOT(runnerConnectionError()));
    qDebug() << "[SyberDebug]: <run> command = " << command;
    m_runner->run(command.toLatin1(), m_sshParameters);
}

void RemoteSyberSignalOperation::finish()
{
    delete m_runner;
    m_runner = 0;
    emit finished(m_errorMessage);
}

/* original code
 *  static QString signalProcessByNameCommandLine(const QString &filePath, int signal)
{
    return QString::fromLatin1(
                "cd /proc; for pid in `ls -d [0123456789]*`; "
                "do "
                "if [ \"`readlink /proc/$pid/exe`\" = \"%1\" ]; then "
                "    kill -%2 $pid;"
                "fi; "
                "done").arg(filePath).arg(signal);
}*
original code*/

// syberos code
static QString signalProcessByNameCommandLine(const QString &filePath, int signal)
{
    QStringList list = filePath.split(QRegExp(QLatin1String("\\s+")));
    const QString str_filePath = list.last();
    QStringList path_name = str_filePath.split(QLatin1String("/"));
    QString app_name = path_name[path_name.size()-1];
    QString ret = QString::fromLatin1(" killall -%2 %1").arg(app_name).arg(signal) ;

    return ret;
}
// syberos code


QString RemoteSyberSignalOperation::killProcessByNameCommandLine(const QString &filePath) const
{
    qDebug() << "[SyberDebug]: <killProcessByNameCommandLine> filePath = " << filePath;
    return QString::fromLatin1("%1; %2").arg(signalProcessByNameCommandLine(filePath, 15),
                                             signalProcessByNameCommandLine(filePath, 9));
}

QString RemoteSyberSignalOperation::interruptProcessByNameCommandLine(const QString &filePath) const
{
    qDebug() << "[SyberDebug]: <interruptProcessByNameCommandLine> filePath = " << filePath;
    return signalProcessByNameCommandLine(filePath, 2);
}

void RemoteSyberSignalOperation::killProcess(qint64 pid)
{
    run(signalProcessByPidCommandLine(pid, 9));
}

void RemoteSyberSignalOperation::killProcess(const QString &filePath)
{
    run(killProcessByNameCommandLine(filePath));
}

void RemoteSyberSignalOperation::interruptProcess(qint64 pid)
{
    run(signalProcessByPidCommandLine(pid, 2));
}

void RemoteSyberSignalOperation::interruptProcess(const QString &filePath)
{
    qDebug() << "[SyberDebug]: <interruptProcess> filePath = " << filePath;
    run(interruptProcessByNameCommandLine(filePath));
}

void RemoteSyberSignalOperation::runnerProcessFinished()
{
    m_errorMessage.clear();
    if (m_runner->processExitStatus() != QSsh::SshRemoteProcess::NormalExit) {
        m_errorMessage = m_runner->processErrorString();
    } else if (m_runner->processExitCode() != 0) {
        m_errorMessage = tr("Exit code is %1. stderr:").arg(m_runner->processExitCode())
                + QLatin1Char(' ')
                + QString::fromLatin1(m_runner->readAllStandardError());
    }
    finish();
}

void RemoteSyberSignalOperation::runnerConnectionError()
{
    m_errorMessage = m_runner->lastConnectionErrorString();
    finish();
}
