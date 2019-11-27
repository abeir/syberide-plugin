#ifndef CHOOSELIST_H
#define CHOOSELIST_H

#include <QObject>
#include <QAbstractListModel>
#include <QDateTime>
//#include <cstoragemanager.h>
#include "developermanager.h"
struct FileInfo {
    QString listName;
    QString package;
    QString pemfile;
    QString password;
    QString keyfile;
    QString effectiveflag;
};

class Chooselist : public QAbstractListModel
{
    Q_OBJECT

   // Q_PROPERTY(QString path READ getDataofList WRITE setDataofList NOTIFY pathChanged)
public:
    enum FileRoles {
        ListNameRole = Qt::UserRole + 1,
        PackageRole,
        PemfileRole,
        PasswordRole,
        KeyfileRole,
        EffectiveflagRole
    };
    explicit Chooselist(QObject *parent = 0);
   // void createTypeGroupBox();
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    /*Q_INVOKABLE void setPath(QString p);*/
    Q_INVOKABLE bool setDataofList(const QModelIndex &index,const QVariant &value, int role);
    Q_INVOKABLE void getDataofList();
     QVector<FileInfo> fileList;
protected:
    QHash<int, QByteArray> roleNames() const;

private:

    QHash<int,QByteArray> roles;
    QString innerStoragePath;
    DeveloperManager * devman;

};

#endif // CHOOSELIST_H
