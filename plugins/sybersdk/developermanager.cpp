#include "developermanager.h"
#include "deviceitem.h"
#include <utils/fileutils.h>
#include <coreplugin/icore.h>
#include <coreplugin/documentmanager.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QDomElement>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextCodec>
#include <QDir>
#include <QtDebug>
//#include <Python.h>
#include <QDomNode>

#include<QApplication>
#include <QMessageBox>

DeveloperManager::DeveloperManager(QObject *parent) :
    QObject(parent)
  ,m_pObjectListModel(NULL)
  ,m_pNetManager(NULL)
  ,m_pLandManager(NULL)
{
    m_pObjectListModel = new ObjectListModel(this);
    m_pNetManager = new QNetworkAccessManager(this);
    connect(m_pNetManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    m_pLandManager = new QNetworkAccessManager(this);
    connect(m_pLandManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(landFinished(QNetworkReply*)));

    QDir path;
    QString file = path.currentPath() + path.separator() + QLatin1String("address");
    QFile fileConfg(file);
    if (!fileConfg.open(QIODevice::ReadOnly|QIODevice::Text)) {
       qDebug() << "can not open file : " << file;
    } else {
        while (!fileConfg.atEnd()) {
            QByteArray array = fileConfg.readLine();
            QTextCodec * codec = QTextCodec::codecForName("GBK");
            QString line = codec->toUnicode(array);
            QString  info = line.trimmed();
            QStringList list = info.split(QLatin1String("="));
            m_strAddress =  list.at(1);
        }
    }

    if(m_strAddress.isEmpty()){
        qDebug() << "m_strAddress is a empty, use default";
        m_strAddress = QLatin1String("https://appstore.syberos.com");
    }
    //qDebug() << "server address: " << m_strAddress;

    QString appPath=Utils::FileUtils::normalizePathName(QCoreApplication::applicationDirPath());
    QDir appdir(appPath);
     appdir.cdUp();
    m_certListconfig = appdir.absolutePath() + QLatin1String("/settings/certificatelistconfig.xml");
    m_certListconfig_fb = appdir.absolutePath() + QLatin1String("/settings/certificatelistconfig_fb.xml");
    qDebug() << "m_certListconfig is " << m_certListconfig;
    qDebug() << "m_certListconfig_fb is " << m_certListconfig_fb;
}

ObjectListModel* DeveloperManager::develListModel() const
{
    return m_pObjectListModel;
}

void DeveloperManager::setDevelListModel(ObjectListModel*model)
{
    m_pObjectListModel = model;
    emit develListModelChanged();
}

void DeveloperManager::deleteDeveloper(int index)
{
    QObject* object = m_pObjectListModel->get(index);
    if(object){
        DeviceItem* item  = qobject_cast<DeviceItem*>(object);
        QString deviceId = item->deviceId();
        QString appId = item->appId();
        deleteFromServer(deviceId,appId);
    }
}

void DeveloperManager::updateDeveloper()
{
    getFromServer();

}

void DeveloperManager::addDeveloper(const QString &deveiceId, const QString & appId)
{
    addToServer(deveiceId,appId);
}

bool DeveloperManager::isLanding()
{
    return token.isEmpty() ? false : true;
}

void DeveloperManager::replyFinished(QNetworkReply*reply)
{
    //qDebug("DeveloperManager::replyFinished");
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "DeveloperManager::replyFinished error" << reply->errorString();
        return;
    }

    m_pObjectListModel->reset();

    QByteArray xmlByteArray  = reply->readAll();
    QString errorStr;
    int l,c;
    if (!m_domDoc.setContent(xmlByteArray,&errorStr,&l,&c)) //modify by fh
    {
        qDebug() << errorStr << "line: " << l << "col: " << c;
        m_domDoc.clear();
        return;
    }

    QDomElement docElem = m_domDoc.documentElement();
    QDomNodeList	list = docElem.childNodes();
    for (int i = 0; i < list.size(); i++)
    {
        DeviceItem* item = new DeviceItem(this);
        QDomNode itemNode  = list.at(i);
        QDomNodeList  attrList = itemNode.childNodes();
        for (int j = 0; j < attrList.size(); j++) {
            QDomElement attrItem = attrList.at(j).toElement();
            if (attrItem.tagName() == QLatin1String("device"))
                item->setDeviceId(attrItem.text());
            else if (attrItem.tagName() == QLatin1String("app"))
                item->setAppId(attrItem.text());
            else if (attrItem.tagName() == QLatin1String("expire"))
                item->setExpTime(attrItem.text());
        }
        m_pObjectListModel->addItem(item);
    }
}

void DeveloperManager::landFinished(QNetworkReply*reply)
{
    //qDebug("DeveloperManager::landFinished");
    if (reply->error() != QNetworkReply::NoError)
    {
        // add by fh
        token.clear();
        emit offLanding();
        //qDebug() << "DeveloperManager::landFinished error:" << reply->errorString();
        return;
    }

    QByteArray jsonByteArray = reply->readAll();
    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(jsonByteArray,&jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (json.isObject())
        {
            QJsonObject jobject= json.object();
            //qDebug() << "token is : " << jobject[QLatin1String("token")].toString();
            token = jobject[QLatin1String("token")].toString();
            emit landSuccess();
        }
    }

}

void DeveloperManager::land(const QString &userName, const QString &password)
{
    QUrl url(m_strAddress + QLatin1String("/api/api-token-auth/"));
    QString str = QLatin1String("username=") + userName + QLatin1String("&password=") + password;
    QByteArray append(str.toStdString().c_str());
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));
    m_pLandManager->post(req, append);
    //m_strUserName = userName;

}

void DeveloperManager::deleteFromServer(const QString &deveiceId, const QString & appId)
{
    if(!isLanding()){
        //emit offLanding(); //comment by fh
        return;
    }
    QUrl url(m_strAddress + QLatin1String("/api/debugkey/delete/"));
    QString value = QLatin1String("Token ") + token;

    QNetworkRequest req;
    QString str = QLatin1String("devid=")+deveiceId + QLatin1String("&appid=")+ appId;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));
    req.setRawHeader("Authorization",value.toUtf8().constData());
    m_pNetManager->post(req, QByteArray(str.toStdString().c_str()));
}

void DeveloperManager::addToServer(const QString &deveiceId, const QString & appId)
{
    if(!isLanding()){
        //emit offLanding(); //comment by fh
        return;
    }
    QUrl url(m_strAddress + QLatin1String("/api/debugkey/add/"));
    QString value = QLatin1String("Token ") + token;

    QNetworkRequest req;
    QString str = QLatin1String("devid=")+deveiceId + QLatin1String("&appid=")+ appId;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));
    req.setRawHeader("Authorization",value.toUtf8().constData());
    m_pNetManager->post(req, QByteArray(str.toStdString().c_str()));
}

void DeveloperManager::getFromServer()
{
    if(!isLanding()){
        //emit offLanding(); //comment by fh
        return;
    }
    QUrl url(m_strAddress + QLatin1String("/api/debugkey/"));
    QString value = QLatin1String("Token ") + token;

    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QLatin1String("application/x-www-form-urlencoded"));
    req.setRawHeader("Authorization",value.toUtf8().constData());
    m_pNetManager->get(req);
}

//void DeveloperManager::saveDocument(QDomDocument doc)
//{
//    QDomElement root = doc.documentElement();
////    if(m_strUserName.isEmpty()){
////        m_strUserName = QLatin1String("defaultName");
////    }
//    QDir path;
//    QString filePath = path.homePath() + path.separator();
//    filePath += QLatin1String("DEBUG.KEY");

//    QFile f(filePath);
//    if (!f.open(QFile::WriteOnly | QFile::Text))
//    {
//      qDebug() << "Open file: "<< filePath <<" to write error";
//      return ;
//    }
//    else
//    {
//        qDebug() << "Open file: "<< filePath <<" successful";
//    }

//    QTextStream out(&f);
//    QTextCodec * codec = QTextCodec::codecForName("GBK");
//    out.setCodec(codec);
//    QDomDocument xmlDoc;
//    QString strHead(QLatin1String("version=\"1.0\" encoding=\"GB2312\""));
//    xmlDoc.appendChild(xmlDoc.createProcessingInstruction(QLatin1String("xml"), strHead));
//    out<<xmlDoc.toString();
//    root.save(out, 3);
//    f.close();
//}

// add by fh
QString DeveloperManager::getDeviceID() const
{
    QString ret;
    char buf[100];
    QString script = Utils::FileUtils::normalizePathName(QCoreApplication::applicationDirPath()) + QLatin1String("/syberos_getDeviceID.sh");
    FILE* fp = popen(script.toLatin1().constData(), "r");
    if (fp == NULL)
    {
        qDebug() << "Run script " << script << "failed";
        return ret;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
        QString str = QString::fromLatin1(buf).simplified();
        if (str.left(9) == QString::fromLatin1("DEVICEID:"))
        {
            ret = str.mid(9).simplified();
            break;
        }
    }
    pclose(fp);
    return ret;
}

bool DeveloperManager::saveDeveloper()
{
    if (m_domDoc.isNull())
        return false;

    QString filename = Core::DocumentManager::getSaveFileName(tr("Save to the file"), QDir::homePath(), QLatin1String(""));
    if (filename.isEmpty())
        return false;

    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
      qDebug() << "Open file "<< filename <<" failed";
      return false;
    }

    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    m_domDoc.documentElement().save(out, 2);
    file.close();
    return true;
}

int DeveloperManager::developerCount() const
{
    return m_pObjectListModel->itemCount();
}

QString DeveloperManager::getSopPackage() const
{
    QStringList list = Core::DocumentManager::getOpenFileNames(tr("sop File (*.sop);; All Files (*.*)"), QDir::homePath());
    return list.isEmpty() ? QLatin1String("") : list.at(0);
}

QString DeveloperManager::getPemFile() const
{
    QStringList list = Core::DocumentManager::getOpenFileNames(tr("Private key (*.pem);; All Files (*.*)"), QDir::homePath());
    return list.isEmpty() ? QLatin1String("") : list.at(0);
}

QString DeveloperManager::getKeyFile() const
{
    QStringList list = Core::DocumentManager::getOpenFileNames(tr("Certificate file (*.cer);; All Files (*.*)"), QDir::homePath());
    return list.isEmpty() ? QLatin1String("") : list.at(0);
}

// 返回值: 0 成功, -1 密码错误 -2 未知错误
// 证书链修改，不再是传入参数，这些参数要从config文件中读取
//原函数定义备份int DeveloperManager::signPackage(const QString &package, const QString &pemfile, const QString &password, const QString &keyfile)
int DeveloperManager::signPackage(const QString &package)
{

    QDomNodeList certificatenodelist;
    searchcertificatelistbytag(QLatin1String("availableflag"),QLatin1String("1"),certificatenodelist);
    if(certificatenodelist.length() <= 0){
        QMessageBox::warning(NULL,tr("error"),tr("There is no valid certificate!"));
        return -1;
    }
    //certificatenodelist中是得到的列表，将其转化为qstring，package，pemfile，password，keyfile
    // Check parameters

    /*QDomNode node0 = certificatenodelist.at(0);
    QString package = node0.toElement().text();*/ //节点内容
    qDebug() << "The sop package to be signed is :"<<package;
    if (package.isEmpty()){
        QMessageBox::warning(NULL,tr("error"),tr("The sop package  to be signed is not selected"));
        return -2;
    }
    QDomNode pem = certificatenodelist.at(1);
    QString pemfile = pem.toElement().text();
    qDebug() << "pemfile:"<<pemfile;
    if(pemfile.isEmpty()){
        QMessageBox::warning(NULL,tr("error"),tr("There is no valid pem file!"));
        return -3;
    }
    QDomNode pw = certificatenodelist.at(3);
    QString password =pw.toElement().text();
    qDebug() << "password:"<<password;
    QDomNode cert = certificatenodelist.at(2);
    QString certfile = cert.toElement().text();
    qDebug() << "certFile:"<<certfile;

    QString appPath = QApplication::applicationDirPath();
    QDir appdir(appPath);
    appdir.cdUp();
    appdir.cdUp();
    QString program = appdir.absolutePath()+QString(QLatin1String("/tools/qtsigner"));
    qDebug() << "The sop release signer path is " << program ;
    m_process = new QProcess;
    connect(m_process, SIGNAL(started()), this, SLOT(programStarted()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

    // Create a temp dir
    m_tmpSopDir = QLatin1String("/tmp/tmpDirForSopSign");
     QDir tmpDir(m_tmpSopDir);
     if (tmpDir.exists()){
         qDebug() << "Warning: " << "The tmp dir for sop sign exists, will be deleted now!";
         if (! tmpDir.removeRecursively()){
             qDebug() << "Error: " << "The old tmp dir for sop sign exists, and can not be deleted ";
             return -4;
         }
     }
     if (!tmpDir.mkdir(m_tmpSopDir)){
          QMessageBox::warning(NULL, tr("error"), tr("The tmp dir [ /tmp/tmpDirForSopSign ] for sop sign create failed! "));
          return -5;
     }

     // extract command
     QString extractCmd = QString::fromLatin1("tar zxvf %1 -C %2").arg( package).arg(m_tmpSopDir);
    system((const char*)extractCmd.toLocal8Bit());

    m_oldPath = QDir::currentPath();
    QDir::setCurrent(m_tmpSopDir);
    QStringList params;
    params << QString::fromLatin1("--sign") << QString::fromLatin1("--pem") << pemfile << QString::fromLatin1("--passwd")
                                            << password << m_tmpSopDir;
    m_process->start(program, params);
    m_process->waitForStarted();

    return 0;
}

void DeveloperManager::programStarted()
{
    disconnect(m_process, SIGNAL(started()), this, SLOT(programStarted()));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(programFinished(int,QProcess::ExitStatus)));
}
void DeveloperManager::programFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    disconnect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(programFinished(int,QProcess::ExitStatus)));
    disconnect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    if((exitCode==0)&&(exitStatus==QProcess::NormalExit))
    {
                delete m_process;
                m_process=NULL;

                QString defPath = QDir::homePath();
                QString filename= Core::DocumentManager::getSaveFileName(tr("Save to the file"), defPath , QLatin1String("sop File (*.sop);; All Files (*.*)"));
                if (filename.isEmpty()){
                    QMessageBox::warning(NULL,tr("error"),tr("The  name of to be saved for sop signing has NOT set!"));
                    return;
                }else{
                    // tar command
                    QString extractCmd = QString::fromLatin1("tar zcvf %1 -C %2 *").arg( filename).arg(m_tmpSopDir);
                   system((const char*)extractCmd.toLocal8Bit());
                   QDir tmpDir(m_tmpSopDir);
                   tmpDir.removeRecursively();
                   emit sopSignSucceed();
                   qDebug() << "The sop package has been signed succeed. ";
                 }
    }else
    {
                 QMessageBox::warning(NULL,tr("error"),tr("The sop signing progress has failed!"));
                 this->killProcess();
                  qDebug() << "exitCode is  " << exitCode;
                  qDebug() << "exitStatus is  " << exitStatus;
    }
     QDir::setCurrent(m_oldPath);
}

void DeveloperManager::processError(QProcess::ProcessError)
{
    disconnect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));

   QMessageBox::warning(NULL,tr("error"),tr("qtsigner start failed!"));
   this->killProcess();
}

void DeveloperManager::killProcess()
{
    if(m_process)
     {
        if((m_process->state()==QProcess::Running)||(m_process->state()==QProcess::Starting))
             m_process->kill();
    }
}


int DeveloperManager::savecertificatelist(const QString &listname, const QString &pemfile, const QString &password, const QString &keyfile)
{
    // Check parameters
    if (listname.isEmpty() || pemfile.isEmpty() || password.isEmpty()){
        qDebug() << "empty!";
        return -2;
    }
    //查询文件中是否已经有相同列表名的节点，若有则报错“列表名称一存在，请从新输入”
    if(searchcertificatelist(listname)==3){
        //列表名已存在，请从新输入
        return -2;
    }

    //插入节点
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomElement node = doc.createElement(QLatin1String("certificate")); //子节点
    root.appendChild(node);

    QDomElement nodeElement = doc.createElement(QLatin1String("Listname"));
    node.appendChild(nodeElement); //子节点的Listname节点(属性)

    QDomText textNode = doc.createTextNode(listname);
    nodeElement.appendChild(textNode); //属性Listname的内容

    nodeElement = doc.createElement(QLatin1String("publiclycertificate"));
    node.appendChild(nodeElement); //子节点的publiclycertificate节点(属性)

    textNode = doc.createTextNode(pemfile);
    nodeElement.appendChild(textNode); //属性publiclycertificate的内容


    nodeElement = doc.createElement(QLatin1String("privatelycertificate"));
    node.appendChild(nodeElement); //子节点的privatelycertificate节点(属性)

    textNode = doc.createTextNode(keyfile);
    nodeElement.appendChild(textNode); //属性privatelycertificate的内容

    nodeElement = doc.createElement(QLatin1String("password"));
    node.appendChild(nodeElement); //子节点的password节点(属性)

    textNode = doc.createTextNode(password);
    nodeElement.appendChild(textNode); //属性password的内容

    nodeElement = doc.createElement(QLatin1String("availableflag"));
    node.appendChild(nodeElement); //子节点的availableflag节点(属性)

    textNode = doc.createTextNode(QLatin1String("0"));
    nodeElement.appendChild(textNode); //属性availableflag的内容

    QFile listconfig_w(m_certListconfig);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    listconfig_w.close();//文件操作结束
    qDebug() << "savecertificatelist success!";
    return 1;
}

int DeveloperManager::deletecertificatelist(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
     if (listname.isEmpty() ){
         qDebug() << "empty!";
         return -2;
     }
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
                 {
                     root.removeChild(docNode);//删除该节点
                     break;
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "deletecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

int DeveloperManager::searchcertificatelist(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
                 {
                     return 3;//找到相同的listname
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    return 1;//没有找到相同的listname
}

int DeveloperManager::availablecertificatelist(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
     if (listname.isEmpty() ){
         qDebug() << "empty!";
         return -2;
     }
     if(!setflagzero())
     {
         qDebug() << "setflagzero feild!";
         return -1;
     }
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)

             QDomNode node = list.at(0);
             QString strNodeTag = node.toElement().tagName(); //节点名称
             QString strNodeText = node.toElement().text(); //节点内容
             if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
              {
                 QDomNode oldnode =  list.at(4);
                  QDomNode nodeflag = list.at(4);
                  //nodeflag.setNodeValue(QLatin1String("1"));
                  nodeflag.firstChild().setNodeValue(QLatin1String("1"));
                  e.replaceChild(nodeflag,oldnode);
                  //nodeflag.toElement().text()=QLatin1String("1");
                 /* list.at(4) = nodeflag;
                  e.childNodes() = list;
                  docNode.toElement() =e ;
                  root.firstChild() = docNode ;
                  doc.documentElement() = root ;*/
                  qDebug() << "tagfind,availablecertificatelist" << "[strNodeText: " << strNodeText << "] " ;
                  break;
              }

         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "availablecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

int DeveloperManager::readhollcertificatelist(QDomDocument &certificatenamelist)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;
    }
    listconfig_r.close();//文件读取结束

    /*QStringList namelist;

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            QDomNode node = list.at(0);
            QString strNodeText = node.toElement().text(); //节点内容
            namelist<<strNodeText;
            qDebug() <<  "filename find: " << filename<<"readhollcertificatelist";
         }
        docNode = docNode.nextSibling(); //下一个节点
    }*/
    certificatenamelist = doc;
    return 1;
}

int DeveloperManager::searchcertificatelistbytag(const QString &tagname,const QString &tagnode,QDomNodeList &certificatenodelist)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (tagname == strNodeTag && tagnode == strNodeText) //名称、内容都相符
                 {
                     certificatenodelist=list;
                     qDebug() <<  "m_certListconfig find: " << m_certListconfig;
                     return 0;//找到
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    qDebug() <<  "m_certListconfig not find: " << m_certListconfig;
    return 1;//没有找到
}

int DeveloperManager::setflagzero()
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            QDomNode oldnode =  list.at(4);
            QDomNode nodeflag = list.at(4);
            nodeflag.firstChild().setNodeValue(QLatin1String("0"));
             e.replaceChild(nodeflag,oldnode);
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "availablecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

int DeveloperManager::savecertificatelist_fb(const QString &listname, const QString &pemfile, const QString &password)
{
    // Check parameters
    if (listname.isEmpty() || pemfile.isEmpty() || password.isEmpty()){
        qDebug() << "empty!";
        return -2;
    }
    //查询文件中是否已经有相同列表名的节点，若有则报错“列表名称一存在，请从新输入”
    if(searchcertificatelist(listname)==3){
        //列表名已存在，请从新输入
        return -2;
    }

    //插入节点
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomElement node = doc.createElement(QLatin1String("certificate")); //子节点
    root.appendChild(node);

    QDomElement nodeElement = doc.createElement(QLatin1String("Listname"));
    node.appendChild(nodeElement); //子节点的Listname节点(属性)

    QDomText textNode = doc.createTextNode(listname);
    nodeElement.appendChild(textNode); //属性Listname的内容

    nodeElement = doc.createElement(QLatin1String("publiclycertificate"));
    node.appendChild(nodeElement); //子节点的publiclycertificate节点(属性)

    textNode = doc.createTextNode(pemfile);
    nodeElement.appendChild(textNode); //属性publiclycertificate的内容

    nodeElement = doc.createElement(QLatin1String("password"));
    node.appendChild(nodeElement); //子节点的password节点(属性)

    textNode = doc.createTextNode(password);
    nodeElement.appendChild(textNode); //属性password的内容

    nodeElement = doc.createElement(QLatin1String("availableflag"));
    node.appendChild(nodeElement); //子节点的availableflag节点(属性)

    textNode = doc.createTextNode(QLatin1String("0"));
    nodeElement.appendChild(textNode); //属性availableflag的内容

    QFile listconfig_w(m_certListconfig_fb);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    listconfig_w.close();//文件操作结束
    qDebug() << "savecertificatelist success!";
    return 1;
}

int DeveloperManager::deletecertificatelist_fb(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
     if (listname.isEmpty() ){
         qDebug() << "empty!";
         return -2;
     }
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
                 {
                     root.removeChild(docNode);//删除该节点
                     break;
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig_fb);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "deletecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

int DeveloperManager::searchcertificatelist_fb(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
     if (listname.isEmpty() ){
         qDebug() << "empty!";
         return -2;
     }
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
                 {
                     return 3;//找到相同的listname
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    return 1;//没有找到相同的listname
}

int DeveloperManager::availablecertificatelist_fb(const QString &listname)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
     if (listname.isEmpty() ){
         qDebug() << "empty!";
         return -2;
     }
     if(!setflagzero_fb())
     {
         qDebug() << "setflagzero feild!";
         return -1;
     }
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)

             QDomNode node = list.at(0);
             QString strNodeTag = node.toElement().tagName(); //节点名称
             QString strNodeText = node.toElement().text(); //节点内容
             if (QLatin1String("Listname") == strNodeTag && listname == strNodeText) //名称、内容都相符
              {
                 QDomNode oldnode =  list.at(3);
                  QDomNode nodeflag = list.at(3);
                  //nodeflag.setNodeValue(QLatin1String("1"));
                  nodeflag.firstChild().setNodeValue(QLatin1String("1"));
                  e.replaceChild(nodeflag,oldnode);
                  //nodeflag.toElement().text()=QLatin1String("1");
                 /* list.at(4) = nodeflag;
                  e.childNodes() = list;
                  docNode.toElement() =e ;
                  root.firstChild() = docNode ;
                  doc.documentElement() = root ;*/
                  qDebug() << "tagfind,availablecertificatelist" << "[strNodeText: " << strNodeText << "] " ;
                  break;
              }

         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig_fb);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "availablecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

int DeveloperManager::readhollcertificatelist_fb(QDomDocument &certificatenamelist)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;
    }
    listconfig_r.close();//文件读取结束


    certificatenamelist = doc;
    return 1;
}

int DeveloperManager::searchcertificatelistbytag_fb(const QString &tagname,const QString &tagnode,QDomNodeList &certificatenodelist)
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            for(int i=0; i<list.count(); i++)
            {
                 QDomNode node = list.at(i);
                 QString strNodeTag = node.toElement().tagName(); //节点名称
                 QString strNodeText = node.toElement().text(); //节点内容
                 if (tagname == strNodeTag && tagnode == strNodeText) //名称、内容都相符
                 {
                     certificatenodelist=list;
                     qDebug() <<  "m_certListconfig_fb find: " << m_certListconfig_fb;
                     return 2;//找到
                 }
             }
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    qDebug() <<  "m_certListconfig_fb not find: " << m_certListconfig_fb;
    return 1;//没有找到
}

int DeveloperManager::setflagzero_fb()
{
    QDomDocument doc;
    QString errorStr;
     int errorLine;
     int errorCol;
    //QString filename = QString(QLatin1String("%1%2")).arg(QDir::homePath()).arg(QLatin1String("/SyberOS-SDK/ide/settings/certificatelistconfig_fb.xml"));
    //qDebug() << "filename:!"<<filename;
    QFile listconfig_r(m_certListconfig_fb);

    if (! listconfig_r.open( QFile::ReadOnly | QFile::Text ) ){
        listconfig_r.close();
        qDebug() << "openfile_read!";
        return -1;
    }
    if (!doc.setContent(&listconfig_r,true,&errorStr,&errorLine,&errorCol)){
        listconfig_r.close();
        qDebug() << errorStr << "line: " << errorLine << "col: " << errorCol;
        return -1;

    }
    listconfig_r.close();//文件读取结束

    QDomElement root = doc.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档，删除节点(只会删除一个)
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            QDomNode oldnode =  list.at(3);
            QDomNode nodeflag = list.at(3);
            nodeflag.firstChild().setNodeValue(QLatin1String("0"));
             e.replaceChild(nodeflag,oldnode);
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    QFile listconfig_w(m_certListconfig_fb);
    if (! listconfig_w.open( QFile::WriteOnly | QFile::Text ) ){
        listconfig_w.close();
        qDebug() << "openfile_write!";
        return -1;
    }
    QTextStream out(&listconfig_w);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);
    qDebug() << "availablecertificatelist success!";
    listconfig_w.close();//文件操作结束
    return 1;
}

// add by ty end

