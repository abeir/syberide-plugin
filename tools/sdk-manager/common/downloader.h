#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include<QNetworkAccessManager>
#include<QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT

public:
    explicit Downloader(QObject *parent = 0);
    QNetworkReply *startDownload(QString urlString,QString fileName);
    void setDestFileName(QString fileName);
    QString getDestFileName() const;

signals:
    void downfinished();

public slots:

private slots:
    void readHttpReplydate();
    void onError(QNetworkReply::NetworkError error);

private:
        QNetworkAccessManager networkManager;
        QString destfileName;
        int errorFlag;

};

#endif // DOWNLOADER_H
