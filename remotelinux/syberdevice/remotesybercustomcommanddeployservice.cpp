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

#include "remotesybercustomcommanddeployservice.h"

#include <utils/qtcassert.h>
#include <ssh/sshremoteprocessrunner.h>

using namespace QSsh;

namespace RemoteLinux {
namespace Internal {
namespace {
enum State { Inactive, Running };
}

class RemoteSyberCustomCommandDeployServicePrivate
{
public:
    RemoteSyberCustomCommandDeployServicePrivate() : state(Inactive), runner(0) { }

    QString commandLine;
    State state;
    SshRemoteProcessRunner *runner;
};

} // namespace Internal

using namespace Internal;


RemoteSyberCustomCommandDeployService::RemoteSyberCustomCommandDeployService(QObject *parent)
    : AbstractRemoteLinuxDeployService(parent), d(new RemoteSyberCustomCommandDeployServicePrivate)
{
}

RemoteSyberCustomCommandDeployService::~RemoteSyberCustomCommandDeployService()
{
    delete d;
}

void RemoteSyberCustomCommandDeployService::setCommandLine(const QString &commandLine)
{
    QTC_ASSERT(d->state == Inactive, return);

    d->commandLine = commandLine;
}

bool RemoteSyberCustomCommandDeployService::isDeploymentPossible(QString *whyNot) const
{
    QTC_ASSERT(d->state == Inactive, return false);

    if (!AbstractRemoteLinuxDeployService::isDeploymentPossible(whyNot))
        return false;
    if (d->commandLine.isEmpty()) {
        if (whyNot)
            *whyNot = tr("No command line given.");
        return false;
    }

    return true;
}

void RemoteSyberCustomCommandDeployService::doDeploy()
{
    QTC_ASSERT(d->state == Inactive, handleDeploymentDone());

    if (!d->runner)
        d->runner = new SshRemoteProcessRunner(this);
    connect(d->runner, SIGNAL(readyReadStandardOutput()), SLOT(handleStdout()));
    connect(d->runner, SIGNAL(readyReadStandardError()), SLOT(handleStderr()));
    connect(d->runner, SIGNAL(processClosed(int)), SLOT(handleProcessClosed(int)));

    emit progressMessage(tr("Starting remote command \"%1\"...").arg(d->commandLine));
    d->state = Running;
    d->runner->run(d->commandLine.toUtf8(), deviceConfiguration()->sshParameters());
}

void RemoteSyberCustomCommandDeployService::stopDeployment()
{
    QTC_ASSERT(d->state == Running, return);

    disconnect(d->runner, 0, this, 0);
    d->runner->cancel();
    d->state = Inactive;
    handleDeploymentDone();
}

void RemoteSyberCustomCommandDeployService::handleStdout()
{
    emit stdOutData(QString::fromUtf8(d->runner->readAllStandardOutput()));
}

void RemoteSyberCustomCommandDeployService::handleStderr()
{
    emit stdErrData(QString::fromUtf8(d->runner->readAllStandardError()));
}

void RemoteSyberCustomCommandDeployService::showInstallInfo(int errorcode)
{
    QString message;
    switch (d->runner->processExitCode()) {
    case 0:
    case 150:
        message = QString::fromUtf8("150: 安装或卸载成功");
        break;
    case 155:
        message = QString::fromUtf8("150: 安装失败");
        break;
    case 156:
        message = QString::fromUtf8("156: 未知安装错误");
        break;
    case 157:
    case 158:
        message = QString::fromUtf8("157: 应用程序未安装");
        break;
    case 159:
        message = QString::fromUtf8("158: 预装系统应用无法卸载");
        break;
    case 160:
        message = QString::fromUtf8("160: 安装程序异常");
        break;
    case 161:
        message = QString::fromUtf8("161: 安装包不存在");
        break;
    case 162:
        message = QString::fromUtf8("162: 无法识别的包类型");
        break;
    case 163:
        message = QString::fromUtf8("163: 签名验证失败");
        break;
    case 164:
        message = QString::fromUtf8("164: 包配置文件不存在");
        break;
    case 165:
        message = QString::fromUtf8("165: 安装包有非法文件");
        break;
    case 166:
        message = QString::fromUtf8("166: 安装失败");
        break;
    case 167:
        message = QString::fromUtf8("167: 应用程序身份标识长度检查失败");
        break;
    case 168:
        message = QString::fromUtf8("168: 磁盘空间不足");
        break;
    case 169:
        message = QString::fromUtf8("169: Release签名认证失败");
        break;
    case 170:
        message = QString::fromUtf8("170: 安装超时");
        break;
    case 171:
        message = QString::fromUtf8("171: 应用程序不在白名单");
        break;
    case 172:
        message = QString::fromUtf8("172: 应用程序开发者不匹配");
        break;
    case 173:
        message = QString::fromUtf8("173: 应用程序签名过期");
        break;
    case 174:
        message = QString::fromUtf8("174: 包配置文件属性描述错误");
        break;
    case 175:
        message = QString::fromUtf8("175: 应用程序账户已经存在");
        break;
    case 176:
        message = QString::fromUtf8("176: 应用程序签名证书不相同");
        break;
    case 177:
        message = QString::fromUtf8("177: 备份应用程序安装目录失败");
        break;
    case 178:
        message = QString::fromUtf8("178: 安装出现错误，回滚已经安装的应用程序失败");
        break;
    case 179:
        message = QString::fromUtf8("179: 安装应用程序动态库失败");
        break;
    case 180:
        message = QString::fromUtf8("180: 为应用程序分配账户失败");
        break;
    case 181:
        message = QString::fromUtf8("181: 检查应用程序安装信息失败");
        break;
    case 182:
        message = QString::fromUtf8("182: 设置应用程序沙盒失败");
        break;
    case 183:
        message = QString::fromUtf8("183: 应用程序安装目录已经存在");
        break;
    case 184:
        message = QString::fromUtf8("184: 注册创建的应用程序账户失败");
        break;

    //185~193 为兼容OS2.01与2.1，main_dev中无效
    case 185:
        message = QString::fromUtf8("185: 证书过期");
        break;
    case 186:
        message = QString::fromUtf8("186: 证书类型不支持");
        break;
    case 187:
        message = QString::fromUtf8("187: 证书为空");
        break;
    case 188:
        message = QString::fromUtf8("188: 证书已经被吊销");
        break;
    case 189:
        message = QString::fromUtf8("189: 证书非官方签发");
        break;
    case 190:
        message = QString::fromUtf8("190: 校验路径不可读或非目录");
        break;
    case 191:
        message = QString::fromUtf8("191: 签名文件CERT.SF被修改或非正常签名");
        break;
    case 192:
        message = QString::fromUtf8("192: MANIFEST.HF 文件被修改或非正常签名");
        break;
    case 193:
        message = QString::fromUtf8("193: 待安装的文件被修改或非正常签名");
        break;

    case 198:
        message = QString::fromUtf8("198: 证书非官方签发");
        break;
    case 199:
        message = QString::fromUtf8("199: 非法的校验路径");
        break;
    case 200:
        message = QString::fromUtf8("200: CERT.SF文件被修改或签名错误");
        break;
    case 201:
        message = QString::fromUtf8("201: MANIFEST.HF内容被修改或签名错误");
        break;
    case 202:
        message = QString::fromUtf8("202: 待安装文件内容被修改");
        break;
    case 203:
        message = QString::fromUtf8("203: MANIFEST.HF证书文件不存在或不可读");
        break;
    case 204:
        message = QString::fromUtf8("204: CERT.SF文件不存在或不可读");
        break;
    case 205:
        message = QString::fromUtf8("205: SIGSF文件不存在或不可读");
        break;
    case 21:
        message = QString::fromUtf8("21: 证书链错误");
        break;
    case 207:
        message = QString::fromUtf8("207: 证书使用超出证书权限");
        break;
    case 208:
        message = QString::fromUtf8("208: 不信任的证书");
        break;
    case 209:
        message = QString::fromUtf8("209: 证书签名验证失败");
        break;
    case 210:
        message = QString::fromUtf8("210: 无效的证书验证机构");
        break;
    case 211:
        message = QString::fromUtf8("211: 证书类型和应用类型不匹配");
        break;
    case 212:
        message = QString::fromUtf8("212: 证书是自签名");
        break;
    case 213:
        message = QString::fromUtf8("213: 证书已经被吊销");
        break;
    case 214:
        message = QString::fromUtf8("214: 证书路径过长");
        break;
    case 215:
        message = QString::fromUtf8("215: 证书过期");
        break;
    case 216:
        message = QString::fromUtf8("216: 根证书过期");
        break;
    case 217:
        message = QString::fromUtf8("217: 证书有效性未知");
        break;
    case 218:
        message = QString::fromUtf8("218: 不支持的证书");
        break;
    case 219:
        message = QString::fromUtf8("219: 证书为空");
        break;

    default:
        message = QString::fromUtf8("安装失败:未知错误");
        break;
    }

    QString str1 = QString::fromUtf8("SOP应用安装: ");

    emit errorMessage(str1 + message);
}

void RemoteSyberCustomCommandDeployService::handleProcessClosed(int exitStatus)
{
    QTC_ASSERT(d->state == Running, return);

    if (exitStatus == SshRemoteProcess::FailedToStart) {
        emit errorMessage(tr("Remote process failed to start."));
    } else if (exitStatus == SshRemoteProcess::CrashExit) {
        emit errorMessage(tr("Remote process was killed by a signal."));
    } else if (d->runner->processExitCode() != 0) {
        if(d->commandLine.contains(QLatin1String("ins-tool")))
            showInstallInfo(d->runner->processExitCode());
        else
            emit errorMessage(tr("Remote process finished with exit code %1.")
            .arg(d->runner->processExitCode()));
    } else {
        emit progressMessage(tr("Remote command finished successfully."));
    }

    stopDeployment();
}

} // namespace RemoteLinux
