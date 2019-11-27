#include "checkupdatethead.h"
#include "constants.h"
#include<QNetworkAccessManager>
#include<QUrl>
#include<QNetworkReply>
#include<QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include<QtXml/QDomNode>
#include<QProcess>
#include<QIODevice>
#include<QThread>
#include<QApplication>
#include<QDir>
#include<QIODevice>
#include<QDebug>

using namespace SyberSDKUpdate::Internal;
using namespace QUpdate;

CheckUpdateThead::CheckUpdateThead(QObject *parent) :
    QObject(parent)
{
    this->downChangelogFlag=0;
    this->enableflag=0;
    this->checkUpdatecount=0;
    timer=new QTimer(this);
    networkmanager=new QNetworkAccessManager(this);
    //  updateXmlName=tr("/tmp/Updates.xml");
    //   proc=NULL;

}

void CheckUpdateThead::run()
{
    connect(timer,SIGNAL(timeout()),this,SLOT(updateChecker()));
    //   timer->start(2000);
}

QDomElement CheckUpdateThead::parseFile(QFile *file,QDomDocument *doc,QString xmlrootTagName,QIODevice::OpenMode mode)
{

    if(!file->exists())
        return QDomElement();

    if(!file->open(mode))
    {
        qDebug() <<"Cannot open file for reading:"
                <<qPrintable(file->errorString());
        return QDomElement();
    }

    //  QDomDocument document;
    QString strError;
    int  errLin = 0, errCol = 0;
    if( !doc->setContent(file,  &strError, &errLin, &errCol) ) {
        qDebug()<<"read xml error"<<errLin<<errCol<<strError;
        return QDomElement();
    }

    if( doc->isNull() ) {
        qDebug()<< "document is null !\n" ;
        return QDomElement();
    }
    QDomElement root = doc->documentElement();
    if(root.tagName()!=xmlrootTagName)
    {
        qDebug()<<QString(QLatin1String("root element %1 unexpected")).arg(root.tagName());
        return QDomElement();
    }


    return root;

}
packageinfo CheckUpdateThead::parseXmlElement(const QDomElement &packageE)
{
    if(packageE.isNull())
        return packageinfo();
    packageinfo info;
    for(int i=0;i<packageE.childNodes().count();i++)
    {
        QDomElement childE=packageE.childNodes().at(i).toElement();
        if(childE.isNull())
            continue;
        if(childE.tagName()==NAME)
            info.name=childE.text();
        if(childE.tagName()==VERSION)
            info.version=childE.text();
    }

    return info;
}

bool CheckUpdateThead::parseXmlRepofile(QString pathfileName)
{

    QFile file(pathfileName);
    if(!file.exists()){
        return false;
    }
    QDomDocument document;
    QDomElement root=parseFile(&file,&document,RePositories,QIODevice::ReadOnly);

    QDomNodeList childNodes=root.childNodes();
    for(int i=0;i<childNodes.count();i++)
    {
        const QDomElement childE=childNodes.at(i).toElement();
        if(childE.isNull())
            continue;
        if(childE.tagName()==URL)
            urlList.append(childE.text());
        if(childE.tagName()==NoTipFlag)
            this->noTipFlag=childE.text().toInt();
        if(childE.tagName()==AppVersion)
            this->localVersion=childE.text();
    }
    file.close();
    return true;
}

void CheckUpdateThead::setDownChangelogFlag(int flag)
{
    this->downChangelogFlag=flag;
}

bool CheckUpdateThead::parseXmlLocalfile(QString pathfileName)
{
    QFile file(pathfileName);
    QDomDocument document;
    QDomElement root=parseFile(&file,&document,Packages,QIODevice::ReadOnly);
    QDomNodeList childNodes=root.childNodes();
    for(int i=0;i<childNodes.count();i++)
    {
        const QDomElement childE=childNodes.at(i).toElement();
        if(childE.isNull())
            continue;
        if(childE.tagName()==Package)
        {
            packageinfo info=parseXmlElement(childE);
            if((!info.name.isEmpty())||(!info.version.isEmpty()))
                localPackages.insert(info.name,info.version);
        }
    }


    file.close();

    if(localPackages.isEmpty())
        return false;

    return true;
}

void CheckUpdateThead::parseXmlremotefile(QString pathfileName)
{
    QFile file(pathfileName);
    QDomDocument document;
    QDomElement root=parseFile(&file,&document,Updates,QIODevice::ReadOnly);
    QDomNodeList childNodes=root.childNodes();
    for(int i=0;i<childNodes.count();i++)
    {
        const QDomElement childE=childNodes.at(i).toElement();
        if(childE.isNull())
            continue;
        if(childE.tagName()==AppVersion)
        {
            this->remoteVersion=childE.text();
        }
        if(childE.tagName()==PackageUpdate)
        {
            packageinfo info=parseXmlElement(childE);
            if((!info.name.isEmpty())||(!info.version.isEmpty()))
                remotePackages.insert(info.name,info.version);
        }
    }
    file.close();

}

void CheckUpdateThead::setRepositoriesXmlpathDir(QString pathDir)
{
    this->repositoriesXmlpathDir=pathDir;
}

void CheckUpdateThead::updateChecker()
{
    timer->stop();
    urlList.clear();

    qDebug() << Q_FUNC_INFO;

    //  QUrl url;
    if(repositoriesXmlpathDir.isEmpty())
        return;

    QString pathfileName=repositoriesXmlpathDir+rePositoriesXml;

    qDebug() << Q_FUNC_INFO << "rePositoriesXml:" << pathfileName;

    if(!parseXmlRepofile(pathfileName)){
        qDebug() << Q_FUNC_INFO << "parse failed:" << pathfileName ;
        emit this->networkError();
        return;
    }


    QString appPath=QApplication::applicationDirPath();
    QDir appdir(appPath);
    appdir.cdUp();
    appdir.cdUp();
    QString componentpathXmlfile=appdir.absolutePath()+componentsXml;

    qDebug() << Q_FUNC_INFO << "componentsXml:" << componentpathXmlfile;

    // QString componentpathXmlfile=QApplication::applicationDirPath()+tr("/components.xml");
    bool parseResult= parseXmlLocalfile(componentpathXmlfile);
    destfileName=UpdatesXmlPathName;

    qDebug() << Q_FUNC_INFO << "parse componentsXml:" << parseResult;

    if(parseResult)
    {
        foreach (QString url, urlList) {

            if(!startDownload(url,UpdatesXmlName))
            {
                qDebug()<<QLatin1String("url is not valid");
                if(checkUpdatecount<=5)
                    timer->start(2000);

                this->checkUpdatecount++;
                break;
            }
            else
            {
                break;
            }
        }
    }else
    {
        emit this->networkError();
    }
}

void CheckUpdateThead::setNoTipFlag(int flag)
{
    this->noTipFlag=flag;
}

void CheckUpdateThead::setEnabelFlag(int flag)
{
    this->enableflag=flag;
}

int CheckUpdateThead::enableFlag()
{
    return this->enableflag;
}

void CheckUpdateThead::processError(QProcess::ProcessError error)
{
    qDebug()<<tr("process error")<<error;
}

void CheckUpdateThead::onError(QNetworkReply::NetworkError error)
{
    QNetworkReply *const reply = qobject_cast<QNetworkReply *>(sender());

    if (error == QNetworkReply::ProxyAuthenticationRequiredError)
        return; // already handled by onProxyAuthenticationRequired
    if (error == QNetworkReply::AuthenticationRequiredError)
        return; // already handled by onAuthenticationRequired

    if (reply) {
        qDebug()<<"network error or file not exist"<< reply->errorString();
        if(this->downChangelogFlag==1)
        {
            this->downChangelogFlag=0;
            timer->stop();
            this->destfileName=NullString;
            emit this->downloadChanglogFileError(reply->errorString());
            return;
        }

        emit this->networkError();
    } else {
        //: %1 is a sentence describing the error
        qDebug()<<"Unknown network error or file not exist"<<error;
        if(this->downChangelogFlag==1)
        {
            this->downChangelogFlag=0;
            timer->stop();
            this->destfileName=NullString;
            emit this->downloadChanglogFileError(QString());
            return;
        }
        emit this->networkError();
    }

    if(this->checkUpdatecount<=5)
        timer->start(2000);
    this->checkUpdatecount++;
}

void CheckUpdateThead::downloadUpdateinfofile()
{
    destfileName=changeLogPathName;
    foreach (QString url, urlList) {

        if(!startDownload(url,changeLogName))
        {
            qDebug()<<"download error";
            break;
        }
    }
}

QString CheckUpdateThead::changleLogFileName()
{
    if(!destfileName.isEmpty())
        return this->destfileName;
    return QString();
}

QNetworkReply * CheckUpdateThead::startDownload(QString urlString,QString fileName)
{
    QUrl url;
    QString httpUrl=urlString+fileName;
    url.setUrl(httpUrl);
    url.setPort(80);
    if(!url.isValid())
        return 0;
    QNetworkReply *reply=networkmanager->get(QNetworkRequest(url));
    connect(reply,SIGNAL(readyRead()),this,SLOT(readHttpReplydate()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
            SLOT(onError(QNetworkReply::NetworkError)));

    return reply;
}

int CheckUpdateThead::compareVersion(const QString &v1, const QString &v2)
{
    // For tests refer VersionCompareFnTest testcase.

    // Check for equality
    if (v1 == v2)
        return 0;

    // Split version numbers across "."
    const QStringList v1_comps = v1.split(QRegExp(tr( "\\.|-")));
    const QStringList v2_comps = v2.split(QRegExp(tr( "\\.|-")));

    // Check each component of the version
    int index = 0;
    while (true) {
        if (index == v1_comps.count() && index < v2_comps.count())
            return -1;
        if (index < v1_comps.count() && index == v2_comps.count())
            return +1;
        if (index >= v1_comps.count() || index >= v2_comps.count())
            break;

        bool v1_ok, v2_ok;
        int v1_comp = v1_comps[index].toInt(&v1_ok);
        int v2_comp = v2_comps[index].toInt(&v2_ok);

        if (!v1_ok) {
            if (v1_comps[index] == tr("x"))
                return 0;
        }
        if (!v2_ok) {
            if (v2_comps[index] == tr("x"))
                return 0;
        }
        if (!v1_ok && !v2_ok)
            return v1_comps[index].compare(v2_comps[index]);

        if (v1_comp < v2_comp)
            return -1;

        if (v1_comp > v2_comp)
            return +1;

        // v1_comp == v2_comp
        ++index;
    }

    if (index < v2_comps.count())
        return +1;

    if (index < v1_comps.count())
        return -1;

    // Controversial return. I hope this never happens.
    return 0;
}

void CheckUpdateThead::addTextChildHelper(QDomNode *node,
                                          const QString &tag,
                                          const QString &text,
                                          const QString &attributeName,
                                          const QString &attributeValue)
{
    QDomNodeList childNodes=node->childNodes();
    for(int i=0;i<childNodes.count();i++)
    {
        const QDomElement childE=childNodes.at(i).toElement();
        if(childE.isNull())
            continue;
        if(childE.tagName()==tag)
        {
            QDomNode textOldnode=childE.firstChild();
            childE.firstChild().setNodeValue(text);

            QDomNode textNewnode=childE.firstChild();

            childNodes.at(i).replaceChild(textNewnode,textOldnode);
            return;
        }
    }

    QDomElement domElement = node->ownerDocument().createElement(tag);
    QDomText domText = node->ownerDocument().createTextNode(text);

    domElement.appendChild(domText);
    if (!attributeName.isEmpty())
        domElement.setAttribute(attributeName, attributeValue);
    node->appendChild(domElement);
}

void CheckUpdateThead::writeNoTipFlagXmlFile(int flag)
{
    QString pathfileName=repositoriesXmlpathDir+rePositoriesXml;
    QDomDocument doc;
    QFile file(pathfileName);
    QDomElement root=parseFile(&file,&doc,RePositories,QIODevice::ReadOnly);
    doc.appendChild(root);

    if(!this->remoteVersion.isEmpty())
        addTextChildHelper(&root,QLatin1String("ApplicationVersion"),this->remoteVersion);

    addTextChildHelper(&root, QLatin1String("NoTipFlag"),QString::number(flag));
    file.close();

    QFile xmlfile(pathfileName);
    if(!xmlfile.open(QIODevice::Truncate|QIODevice::WriteOnly))
    {
        qDebug() <<"Cannot open file for reading:"
                <<qPrintable(xmlfile.errorString());
        return ;
    }

    xmlfile.write(doc.toByteArray(4));
    xmlfile.close();

}

void CheckUpdateThead::readHttpReplydate()
{

    QNetworkReply *const reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    static QByteArray buffer(16384,'\0');
    QFile file(destfileName);
    if(file.exists())
        file.remove();

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() <<"Cannot open file for writing:"
                <<qPrintable(file.errorString());
        return;
    }
    while(reply->bytesAvailable())
    {
        const qint64 read=reply->read(buffer.data(),buffer.size());
        qint64 written=0;
        while(written< read){
            const qint64 numWritten=file.write(buffer.data()+written,read-written);
            if(numWritten<0){
                const QString error=file.errorString();
                const QString filename=file.fileName();
                qDebug() <<"Cannot download file:"
                        <<qPrintable(error)<<qPrintable(filename);
                return;
            }
            written+=numWritten;
        }
    }
    file.close();

    if(destfileName!=UpdatesXmlPathName)
    {
        emit this->writeChanglog();
        return;
    }

    parseXmlremotefile(destfileName);

    if(this->enableflag==0)
    {
        if(this->noTipFlag==1)
        {

            if((!this->localVersion.isEmpty())&&(!this->remoteVersion.isEmpty()))
            {
                int  comparesult=this->compareVersion(this->remoteVersion,this->localVersion);
                if(comparesult<=0)
                {
                    if(this->checkUpdatecount<=5)
                        timer->start(2000);
                    this->checkUpdatecount++;
                    return;
                }
            }
            else
            {
                if(this->checkUpdatecount<=5)
                    timer->start(2000);
                this->checkUpdatecount++;
                return;
            }
        }
    }

    if(localPackages.isEmpty()||remotePackages.isEmpty())
        return;

    QHashIterator<QString,QString>itator(localPackages);
    int result=0;
    while(itator.hasNext())
    {
        itator.next();
        QString name=itator.key();
        if(remotePackages.contains(name))
        {
            result=this->compareVersion(remotePackages.value(name),localPackages.value(name));
            if(result==1)
            {
                break;
            }
        }
    }

    if(result==1)
    {
        remotePackages.clear();
        localPackages.clear();
        emit update();
    }
    else
    {
        remotePackages.clear();
        localPackages.clear();
        emit this->NoUpdate();
        qDebug()<<tr("No Update");
        if(this->checkUpdatecount<=5)
            timer->start(2000);
        this->checkUpdatecount++;
    }
}

QTimer * CheckUpdateThead::Updatetimer()
{

    return this->timer;
}

