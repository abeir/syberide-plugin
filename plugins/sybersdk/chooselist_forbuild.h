#ifndef CHOOSELIST_FORBUILD_H
#define CHOOSELIST_FORBUILD_H

#include <QObject>
#include <QAbstractListModel>
#include <QDateTime>
//#include <cstoragemanager.h>
#include "developermanager.h"
struct FileInfo_fb {
    QString listName;
    QString pemfile;
    QString password;
    QString effectiveflag;
};

class Chooselist_forbuild : public QAbstractListModel
{
    Q_OBJECT

   // Q_PROPERTY(QString path READ getDataofList WRITE setDataofList NOTIFY pathChanged)
public:
    enum FileRoles {
        ListNameRole = Qt::UserRole + 1,
        PemfileRole,
        PasswordRole,
        EffectiveflagRole
    };
    explicit Chooselist_forbuild(QObject *parent = 0);
   // void createTypeGroupBox();
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    /*Q_INVOKABLE void setPath(QString p);*/
    Q_INVOKABLE bool setDataofList(const QModelIndex &index,const QVariant &value, int role);
    Q_INVOKABLE void getDataofList();
     QVector<FileInfo_fb> fileList;
protected:
    QHash<int, QByteArray> roleNames() const;

private:

    QHash<int,QByteArray> roles;
    QString innerStoragePath;
    DeveloperManager * devman;

};

#endif // CHOOSELIST_FORBUILD_H
