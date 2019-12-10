#ifndef SSHHANDLER_H
#define SSHHANDLER_H

#include <QObject>
#include <QMetaObject>
#include <QByteArray>
#include <QList>

#include <ssh/sshconnection.h>
#include "../syberosdevice_global.h"

namespace QSsh
{
    class SshConnection;
    class SftpChannel;
    class SshConnectionParameters;
    class SshRemoteProcess;
    typedef quint32 SftpJobId;
}

namespace SyberOSDevice {
namespace Internal {

        class SYBEROSDEVICESHARED_EXPORT SshHandler : public QObject
        {
            Q_OBJECT
        public:
            enum SshError{
                ConnectionError = -4
                , SftpError = -3
                , ProcessError = -2
                , Disconnect = -1
                , AllCorrect = 0
                , ConnectionCorrect = 1
                , SftpCorrect = 2
                , ProcessCorrect = 3
            };

            Q_ENUMS(SshError)

            explicit SshHandler(QObject *parent = 0);
            ~SshHandler();

            void sshInit();
            //authType: false passwd;  true public key
            void sshInit(QString strHost, QString userName, QString strAuth, bool authType=false, int port=22);
            void sshPullFile(QString absolutePath = QStringLiteral(""), QString localPath = QStringLiteral(""), bool isToDel = true);
            void sshUploadDir(QString remotePath, QString localPath, bool isToDel);
            void sshUploadFile(QString remotePath, QString localPath, bool isToDel);
            //void disConnect();
            void sshStop();
            void sshListDir(QString remotePath);
            void sshExecProcess(QString cmd = QString());
            void sshExecPythonProcess(QString cmd);

            SshError getState(){ return m_iErrorNum; }
            void clearImage();

        signals:
            void error(QString errMsg);
            void message(QString message);
            void finishConn(int connectionType);
            void remoteProcessReturned(QString returnVal);
            void sftpFinished(QString localPath);
            void remoteProcessFinshed(int error, QString errMsg);

        private:
            void initSftp();
            void sshAssignRemoteProcess(QString command = QStringLiteral(""));
            void handleProcessClosed(int exitStatus);
            void errorHandler();

        private:
            QSsh::SshConnection      *m_sshConnection;
            QSsh::SftpChannel        *m_sshSftp;

            QSsh::SshConnectionParameters  m_param;
            QSsh::SshRemoteProcess   *m_process;
            QByteArray               m_remoteStderr;
            QByteArray               m_remoteStdout;

            SshError                 m_iErrorNum;
        };
    }
}

#endif // SSHHANDLER_H
