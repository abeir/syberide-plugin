#ifndef DEVELOPERMANAGER_H
#define DEVELOPERMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QProcess>
#include "objectlistmodel.h"

class DeveloperManager : public QObject
{
    Q_OBJECT

public:
    explicit DeveloperManager(QObject *parent = 0);
    Q_PROPERTY(ObjectListModel* develListModel READ develListModel WRITE setDevelListModel NOTIFY develListModelChanged)

    ObjectListModel* develListModel() const;
    void setDevelListModel(ObjectListModel*model);

    Q_INVOKABLE void deleteDeveloper(int index);
    Q_INVOKABLE void updateDeveloper();
    Q_INVOKABLE void addDeveloper(const QString &deveiceId, const QString & appId);
    Q_INVOKABLE void land(const QString &userName, const QString &password);
    Q_INVOKABLE bool isLanding();

    // add by fh
    Q_INVOKABLE QString getDeviceID() const;
    Q_INVOKABLE bool saveDeveloper();
    Q_INVOKABLE int developerCount() const;
    Q_INVOKABLE QString getSopPackage() const;
    Q_INVOKABLE QString getPemFile() const;
    Q_INVOKABLE QString getKeyFile() const;
    //Q_INVOKABLE int signPackage(const QString &package, const QString &pemfile, const QString &password, const QString &keyfile);
    Q_INVOKABLE int signPackage(const QString &package);
    Q_INVOKABLE int savecertificatelist(const QString &listname, const QString &pemfile, const QString &password, const QString &keyfile);
    Q_INVOKABLE int deletecertificatelist(const QString &listname);
    Q_INVOKABLE int searchcertificatelist(const QString &listname);
    Q_INVOKABLE int availablecertificatelist(const QString &listname);
    Q_INVOKABLE int  readhollcertificatelist(QDomDocument &certificatenamelist);
    Q_INVOKABLE int searchcertificatelistbytag(const QString &tagname,const QString &tagnode,QDomNodeList &certificatenodelist);
    Q_INVOKABLE int setflagzero();
    Q_INVOKABLE int savecertificatelist_fb(const QString &listname, const QString &pemfile,const QString &password);
    Q_INVOKABLE int deletecertificatelist_fb(const QString &listname);
    Q_INVOKABLE int searchcertificatelist_fb(const QString &listname);
    Q_INVOKABLE int availablecertificatelist_fb(const QString &listname);
    Q_INVOKABLE int  readhollcertificatelist_fb(QDomDocument &certificatenamelist);
    Q_INVOKABLE int searchcertificatelistbytag_fb(const QString &tagname,const QString &tagnode,QDomNodeList &certificatenodelist);
    Q_INVOKABLE int setflagzero_fb();
    //Q_INVOKABLE int modifycertificatelist(const QString &tagname,const QString &tagnode,QDomNodeList &certificatenodelist);
    void deleteFromServer(const QString &deveiceId, const QString & appId);
    void addToServer(const QString &deveiceId, const QString & appId);
    void getFromServer();

signals:
    void develListModelChanged();
    void landSuccess();
    void offLanding();
    void sopSignSucceed();

private slots:
    void replyFinished(QNetworkReply*reply);
    void landFinished(QNetworkReply*reply);

    void programStarted();
    void programFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError);


private:
    ObjectListModel* m_pObjectListModel;
    QNetworkAccessManager *m_pNetManager;//get,update,delete
    QNetworkAccessManager *m_pLandManager;//land
    QString m_strAddress;
    QString token;
    //QString m_strUserName;
    QDomDocument m_domDoc; //add by fh

    QString m_certListconfig;
    QString m_certListconfig_fb;

    QProcess* m_process = nullptr;
    QString m_oldPath;
    QString m_tmpSopDir;

    void killProcess();
};

#endif // DEVELOPERMANAGER_H
