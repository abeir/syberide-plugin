#ifndef CHECKUPDATETHEAD_H
#define CHECKUPDATETHEAD_H

#include<QThread>
#include<QTimer>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include <QtXml/QDomElement>
#include<QProcess>
#include<QFile>
#include<QIODevice>

namespace SyberSDKUpdate {
namespace Internal {
struct packageinfo{
    QString name;
    QString  version;
};
class CheckUpdateThead : public QObject
{
    Q_OBJECT
public:
    explicit CheckUpdateThead(QObject *parent = nullptr);

    bool parseXmlRepofile(QString fileName);
    QDomElement parseFile(QFile *file,QDomDocument *doc,QString xmlrootTagName,QIODevice::OpenMode mode);
    packageinfo parseXmlElement(const QDomElement &packageE);
    bool parseXmlLocalfile(QString pathfileName);
    void parseXmlremotefile(QString pathfileName);
    int compareVersion(const QString &v1, const QString &v2);
    void downloadUpdateinfofile();
    QString changleLogFileName();
    void setDownChangelogFlag(int flag);
    void setRepositoriesXmlpathDir(QString pathDir);
    void writeNoTipFlagXmlFile(int flag);
    void setNoTipFlag(int flag);
    void setEnabelFlag(int flag);
    int enableFlag();
    void run();

signals:
    void update();
    void NoUpdate();
    void writeChanglog();
    void downloadChanglogFileError(const QString errorinfo);
    void networkError();

public slots:
    void updateChecker();

private slots:
    void readHttpReplydate();
    void onError(QNetworkReply::NetworkError error);

    QTimer * Updatetimer();

    void processError(QProcess::ProcessError error);

private:
        QNetworkReply * startDownload(QString urlString,QString fileName);
        void addTextChildHelper(QDomNode *node,
                                      const QString &tag,
                                      const QString &text,
                                      const QString &attributeName = QString(),
                                      const QString &attributeValue = QString());

private:
    QTimer *timer;
    QNetworkAccessManager *networkmanager;
   // QNetworkReply*reply;
    QStringList urlList;
    QHash<QString,QString> localPackages;
    QHash<QString,QString>remotePackages;
    //QString updateXmlName;
    QString destfileName;
    QString remoteVersion;
    QString localVersion;
    QString repositoriesXmlpathDir;
    int downChangelogFlag;
    int noTipFlag;
    int enableflag;
    int checkUpdatecount;

};

}
}

#endif // CHECKUPDATETHEAD_H
