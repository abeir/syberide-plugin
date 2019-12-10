#include "sshhandler.h"
#include "ssh/sftpchannel.h"
#include "ssh/sshconnectionmanager.h"
#include "ssh/sshremoteprocess.h"

#include <QtDebug>
#include <QDateTime>
#include <QGlobalStatic>
#include <QString>
#include <QDir>



using namespace QSsh;
using namespace QSsh::Internal;
//using namespace SyberOSDevice::Internal;

static const QString S_SCREENSHOT_PATH(QStringLiteral("/home/user/ScreenShot"));
static const QString S_LOCAL_SCREENSHOT_PATH(QDir::homePath() + QStringLiteral("/.SyberSDK"));
static const QString S_COMMAND(QStringLiteral("dbus-send --system  --print-reply=literal --dest=com.syberos.compositor /com/syberos/compositor com.syberos.compositor.CompositorInterface.captureScreen"));

namespace SyberOSDevice {
namespace Internal {

SshHandler::SshHandler(QObject *parent)
    : QObject(parent)
    , m_sshConnection(nullptr)
    , m_sshSftp(nullptr)
    , m_process(nullptr)
    , m_iErrorNum(AllCorrect)
{
}

void SshHandler::sshStop()
{
    if(m_sshConnection)
    {
        if(m_sshConnection->state() == SshConnection::Connected)
        {
            m_sshConnection->disconnectFromHost();
        }
        releaseConnection(m_sshConnection);
        m_sshConnection = NULL;
    }
    qDebug() << tr("ssh stopped ");
}

SshHandler::~SshHandler()
{
    sshStop();
    m_sshConnection=nullptr;
    m_sshSftp=nullptr;
    m_process=nullptr;
}

void SshHandler::sshInit(QString strHost, QString userName, QString strAuth, bool authType, int port)
{
    m_param.userName = userName;
    m_param.host = strHost;
    if(authType)
    {
        m_param.privateKeyFile = strAuth;
        m_param.authenticationType = SshConnectionParameters::AuthenticationTypePublicKey;

    }
    else
    {
        m_param.password = strAuth;
        m_param.port = port;
        m_param.authenticationType = SshConnectionParameters::AuthenticationTypePassword;
    }

    m_param.timeout = 30;
    m_sshConnection = acquireConnection(m_param);
    if(!m_sshConnection)
    {
        emit error(tr("connection error "));
        m_iErrorNum = ConnectionError;
        return ;
    }

    disconnect(m_sshConnection, 0, this, 0);
    if(m_sshConnection->errorState() != QSsh::SshNoError)
    {
//        QString errorMsg = m_sshConnection->errorString();
//        QSsh::SshError err = m_sshConnection->errorState();
        emit error(m_sshConnection->errorString());
        m_iErrorNum = ConnectionError;
        return ;
    }


    connect(m_sshConnection, &SshConnection::error, [this](){
       emit error( tr("connection error %1   %2").arg(m_sshConnection->errorState()).arg(m_sshConnection->errorString()));
        m_iErrorNum = ConnectionError;
        //m_sshSftp = m_sshConnection->createSftpChannel().data();
    } );


    connect(m_sshConnection, &SshConnection::connected, [this](){
        qDebug() << tr("connection setup");
        m_iErrorNum = ConnectionCorrect;
        emit finishConn(m_iErrorNum);
        initSftp();
    } );

//    connect(m_sshConnection, &SshConnection::disconnected, [this](){
//        qDebug() << tr("connection disconnect");
//        m_iErrorNum = Disconnect;
//        emit finishConn(m_iErrorNum);
//    } );
    if(m_sshConnection->state() == SshConnection::Unconnected)
        m_sshConnection->connectToHost();
    else
    {
        qDebug()<< tr("ssh connection already exists!");
        m_iErrorNum = ConnectionCorrect;
        emit finishConn(m_iErrorNum);
        initSftp();
    }
}

void SshHandler::sshInit()
{
    sshInit(QStringLiteral("192.168.100.100"), QStringLiteral("developer"), QStringLiteral("system"), false, 22);
}

void SshHandler::sshListDir(QString remotePath)
{
    Q_ASSERT(m_sshSftp);
    if(!m_sshSftp)
    {
        m_iErrorNum = SftpError;
        return ;
    }
    m_sshSftp->disconnect(SIGNAL( finished(QSsh::SftpJobId, const QString &)));
    m_sshSftp->disconnect(SIGNAL( fileInfoAvailable(QSsh::SftpJobId , const QList<QSsh::SftpFileInfo> &)));

    SftpJobId jobId = m_sshSftp->listDirectory(remotePath);
//    m_tmpConnections.push_back(
    connect(m_sshSftp, &SftpChannel::fileInfoAvailable, [this, jobId](QSsh::SftpJobId job, const QList<QSsh::SftpFileInfo> &fileInfoList){
        if(job == jobId)
        {
            qDebug() << tr("file info: ");
            foreach(QSsh::SftpFileInfo info, fileInfoList)
            {
                qDebug() << tr("name : ")+ info.name;
                qDebug() << tr("\ttype : %1").arg(info.type);
                qDebug() << tr("\tpermissions : %1").arg(info.permissions);
                qDebug() << tr("\tsize : %1").arg(info.size);
            }
        }
    });
}


void SshHandler::sshExecProcess(QString cmd)
{
    if(cmd.isEmpty())
        cmd = S_COMMAND;
    sshAssignRemoteProcess(cmd);
    m_process->start();
}

void SshHandler::sshExecPythonProcess(QString cmd)
{
    sshAssignRemoteProcess(cmd);
    //m_process->addToEnvironment("PATH", "/var/data/coverity/bin");
    m_process->addToEnvironment("PATH", "/opt/cov-analysis-linux64-7.7.0/bin");
    connect(m_process, &SshRemoteProcess::aboutToClose, [this](){
//        m_process->disconnect(SIGNAL(readyReadStandardOutput()));
//        m_process->disconnect(SIGNAL(readyReadStandardError()));

        //m_remoteStderr = m_process->readAllStandardError();
       //emit error(tr("error : ") + QString::fromUtf8(m_remoteStderr));
        qDebug() << tr("python: about to close ");
        emit remoteProcessFinshed(0, QStringLiteral("python complete"));
    });

    m_process->start();

}

void SshHandler::clearImage()
{
    QFileInfoList fileInfos = QDir(S_LOCAL_SCREENSHOT_PATH).entryInfoList(QStringList() << QStringLiteral("*.png"), QDir::Files);
    qDebug() << tr("file count %1").arg(fileInfos.size());
    if(fileInfos.size() == 0)        return ;
    foreach(QFileInfo info, fileInfos)
    {
        if(info.completeSuffix().toLower() == QStringLiteral("png"))
        {
            qDebug() << info.absoluteFilePath();
            info.dir().remove(info.absoluteFilePath());
        }
    }
}

void SshHandler::sshPullFile(QString absolutePath, QString localPath, bool isToDel)
{
    Q_ASSERT(m_sshSftp);
    if(!m_sshSftp)
    {
        m_iErrorNum = SftpError;
        return ;
    }

    if(absolutePath.isEmpty())
        absolutePath =QString::fromUtf8(m_remoteStdout.trimmed());
    if(localPath.isEmpty())
        localPath = S_LOCAL_SCREENSHOT_PATH;

    Q_ASSERT(m_sshSftp && m_sshSftp->state() == SftpChannel::Initialized);


    localPath = localPath+tr("/%1.png").arg(QDateTime::currentDateTime().toString(QStringLiteral("hms-yymmdd")));
    qDebug() << tr("information: ") + localPath;

    m_sshSftp->disconnect(SIGNAL( finished(QSsh::SftpJobId, const QString &)));
    SftpJobId jobId = m_sshSftp->downloadFile(absolutePath, localPath, SftpOverwriteExisting);
        connect(m_sshSftp, &SftpChannel::finished,[this, jobId, absolutePath, isToDel, localPath](QSsh::SftpJobId id,QString errorString){
            SftpJobId jobId2 = 0;
            if(id == jobId)
            {
                qDebug()<<tr("file transfer succeed : ")+  localPath;
                if(isToDel)
                    jobId2 = m_sshSftp->removeFile(absolutePath);
                emit sftpFinished(localPath);
            }
            else if(id == jobId2 && jobId2 != 0)
            {
                qDebug()<<tr("file delete succeed");
            }
        });
}

void SshHandler::sshUploadDir(QString remotePath, QString localPath, bool isToDel)
{
    Q_ASSERT(m_sshSftp);
    if(!m_sshSftp)
    {
        m_iErrorNum = SftpError;
        return ;
    }

    if(remotePath.isEmpty() || localPath.isEmpty())
    {
        emit error(tr("file path is empty !"));
        return ;
    }

    if(m_sshSftp->state() != SftpChannel::Initialized)
        initSftp();

    QSsh::SftpJobId jobIdRm = 0;
    QSsh::SftpJobId jobIdUpload = 0;
    QSsh::SftpJobId jobIdDir = 0;

    QString dirname = QDir(localPath).dirName();

    QString cmd =
    tr("    if [ ! -d %1 ]; then\n"
        "\tmkdir %1\n"
        "fi")
       .arg(remotePath + tr("/") + dirname);


    //tr("rm -rf %1").arg(remotePath + tr("/") + s_dirSuffix)
    sshAssignRemoteProcess(cmd);
    connect(m_process, &SshRemoteProcess::aboutToClose, [this, &jobIdUpload, jobIdDir, localPath, remotePath]{
        emit remoteProcessReturned(tr("process: about to close"));
        jobIdUpload = m_sshSftp->uploadDir(localPath, remotePath);
        qDebug()<< tr("upload job id : %1").arg(jobIdUpload);
        //m_process->disconnect(SIGNAL());
    });

    //isDel = true;
    m_process->start();
    //return ;

    connect(m_sshSftp, &SftpChannel::dataAvailable, [this, jobIdUpload](QSsh::SftpJobId job, const QString &data){
        emit message(data);
    });

    connect(m_sshSftp, &SftpChannel::finished
            ,[this, &jobIdUpload, jobIdDir,isToDel]
                (QSsh::SftpJobId id,QString err){

        QString errorString = tr("%2: %1").arg(err);
        if(!err.isEmpty())
        {
            if(id == jobIdUpload)
                errorString = errorString.arg(tr("upload file error "));
            else
                errorString = errorString.arg(id);
                emit error(errorString);
        }
        if(id != jobIdDir)
            emit sftpFinished(tr("finished jobid %1, message  %2").arg(id).arg(tr("upload file")));
    });

}

void SshHandler::sshUploadFile(QString remotePath, QString localPath, bool isToDel)
{
    Q_ASSERT(m_sshSftp);
    if(!m_sshSftp)
    {
        m_iErrorNum = SftpError;
        return ;
    }

    if(remotePath.isEmpty() || localPath.isEmpty())
    {
        emit error(tr("file path is empty !"));
        return ;
    }

    Q_ASSERT(m_sshSftp && m_sshSftp->state() == SftpChannel::Initialized);
    m_sshSftp->disconnect(SIGNAL( finished(QSsh::SftpJobId, const QString &)));
    m_sshSftp->disconnect(SIGNAL( fileInfoAvailable(QSsh::SftpJobId , const QList<QSsh::SftpFileInfo> &)));
    SftpJobId jobId = m_sshSftp->uploadFile(localPath, remotePath, QSsh::SftpOverwriteExisting);
    connect(m_sshSftp, &SftpChannel::finished, [this, jobId, localPath](QSsh::SftpJobId id, const QString &err){
//        if(id == jobId)
            emit sftpFinished(localPath);
    });
}

void SshHandler::initSftp()
{
    m_sshSftp = m_sshConnection->createSftpChannel().data();
    disconnect(m_sshSftp, 0, this, 0);
    connect(m_sshSftp, &SftpChannel::initialized, [this](){
        m_iErrorNum = SftpCorrect;
        emit finishConn(SftpCorrect);
    });
    connect(m_sshSftp, &SftpChannel::channelError, [this](const QString &reason){
        m_iErrorNum = SftpError;
        emit error(tr("sftp error : ") + reason);
    });

    m_sshSftp->initialize();

}

void SshHandler::errorHandler()
{
    Q_ASSERT(m_sshSftp);
}


void SshHandler::sshAssignRemoteProcess(QString command)
{
    m_process = m_sshConnection->createRemoteProcess(command.toLatin1()).data();
    m_process->disconnect();
    //disconnect(m_process, 0, this, 0);
    //m_process->setReadChannel(QProcess::StandardError);
    connect(m_process, &SshRemoteProcess::closed, [this](){
        m_process->disconnect(SIGNAL(readyReadStandardOutput()));
        m_process->disconnect(SIGNAL(readyReadStandardError()));
    });

    connect(m_process, &SshRemoteProcess::readyReadStandardOutput, [this](){
        if(m_process->isReadable())
        {
            m_remoteStdout = m_process->readAllStandardOutput().trimmed();
            if(!m_remoteStdout.isEmpty())
            {
                emit remoteProcessReturned(QString::fromUtf8(m_remoteStdout));
            }
        }
        // m_process->close();
    });

    connect(m_process, &SshRemoteProcess::readyReadStandardError, [this](){
        if(m_process->isReadable())
        {
            m_remoteStderr = m_process->readAllStandardError();
            if(!m_remoteStderr.isEmpty())
            {
                emit error(QString::fromUtf8(m_remoteStderr));
            }
        }
    });

//    connect(m_process, &QIODevice::aboutToClose, [this](){
//        //m_remoteStderr = m_process->readAllStandardError();
//       //emit error(tr("error : ") + QString::fromUtf8(m_remoteStderr));
//        m_process->close();
//    });

}

void SshHandler::handleProcessClosed(int exitStatus)
{
        QString errMsg;
        switch (exitStatus) {
        case SshRemoteProcess::FailedToStart:
            errMsg = tr("Could not start remote process: %1").arg(m_process->errorString());
            break;
        case SshRemoteProcess::CrashExit:
            errMsg = tr("Remote process crashed: %1").arg(m_process->errorString());
            break;
        case SshRemoteProcess::NormalExit:
                errMsg = tr("Remote process failed; exit code was %1.").arg(m_process->exitCode());
            break;
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid exit status");
        }

        if (!errMsg.isEmpty()) {
            if (!m_remoteStderr.isEmpty()) {
                errMsg += QLatin1Char('\n');
                errMsg += tr("Remote error output was: %1")
                    .arg(QString::fromUtf8(m_remoteStderr));
            }
            emit error(errMsg);
        }
    qDebug() << tr("process closed");
}

}
}
