#include "downloader.h"
#include<QUrl>
#include<QByteArray>
#include<QFile>
#include<QMessageBox>

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{

}


QNetworkReply *Downloader::startDownload(QString urlString,QString fileName)
{
    QUrl url;
    QString httpUrl=urlString+fileName;
    url.setUrl(httpUrl);
    url.setPort(80);
    if(!url.isValid())
    {
        QMessageBox::warning(0, QMessageBox::tr("The url is wrong!"), QMessageBox::tr("The url is wrong %1!").arg(httpUrl));
        return NULL;
    }
     QNetworkReply *reply=networkManager.get(QNetworkRequest(url));
    // connect(reply,SIGNAL(readyRead()),this,SLOT(readHttpReplydate()));
     connect(reply,SIGNAL(finished()),this,SLOT(readHttpReplydate()));
     connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
     return reply;

}

void Downloader::readHttpReplydate()
{
    QNetworkReply *const reply = qobject_cast<QNetworkReply *>(sender());

    if(!reply)
        return;

    static QByteArray buffer(16384,'\0');
    QFile file(destfileName);
     if(file.exists())
            file.remove();

     if(!file.open(QIODevice::WriteOnly)) {
           qDebug()<<"Cannot open file for writing:"
                            <<qPrintable(file.errorString());
           return;
     }
     while(reply->bytesAvailable()){
         const qint64 read=reply->read(buffer.data(),buffer.size());
         qint64 written=0;
         while(written<read){
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

   emit this->downfinished();
}

void Downloader::setDestFileName(QString fileName)
{
    this->destfileName=fileName;
}

QString Downloader::getDestFileName() const
{
    return this->destfileName;
}

void Downloader::onError(QNetworkReply::NetworkError error)
{
    QNetworkReply *const reply = qobject_cast<QNetworkReply *>(sender());
    if (error == QNetworkReply::ProxyAuthenticationRequiredError)
        return; // already handled by onProxyAuthenticationRequired
    if (error == QNetworkReply::AuthenticationRequiredError)
        return; // already handled by onAuthenticationRequired
    if (reply) {
               qDebug()<<"network error or file not exist"<< reply->errorString();
           //    this->errorFlag=1;
               if(QFile(destfileName).exists())
                         QFile(destfileName).remove();
               QMessageBox::warning(0, QMessageBox::tr("The network is wrong!"), QMessageBox::tr("The network is wrong,the wrong error is %1!").arg(reply->errorString()));
    }else{
      //  this->errorFlag=1;
        if(QFile(destfileName).exists())
                  QFile(destfileName).remove();
         qDebug()<<"Unknown network error"<<error;
    }

}


