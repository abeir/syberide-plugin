#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QMimeDatabase>
//#include <QAbstractListModel>
#include <QAbstractItemModel>
#include "chooselist.h"

//#include <environment.h>
Chooselist::Chooselist(QObject *parent) : QAbstractListModel(parent)
{
    qDebug() << "enter chooselist's construct function......";
//    QHash<int, QByteArray> roles;
    roles[ListNameRole] = "listName";
    roles[PackageRole] = "package";
    roles[PemfileRole] = "pemfile";
    roles[PasswordRole] = "password";
    roles[KeyfileRole] = "keyfile";
    roles[EffectiveflagRole] = "effectiveflag";
    devman = new DeveloperManager;
    //setRoleNames(roles);
    qDebug() << "#####1######";
}

QHash<int, QByteArray> Chooselist::roleNames() const
{
    return roles;
}

int Chooselist::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fileList.count();
}

Qt::ItemFlags Chooselist::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant Chooselist::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 ||
            index.row() >= fileList.count())
        return QVariant();

    FileInfo fi = fileList.at(index.row());
    switch (role) {
    case ListNameRole:
        return fi.listName;
    case PemfileRole:
        return fi.pemfile;
    case PasswordRole:
        return fi.password;
    case KeyfileRole:
        return fi.keyfile;
    case EffectiveflagRole:
        return fi.effectiveflag;
    default:
        break;
    }

    return QVariant();
}

bool Chooselist::setDataofList(const QModelIndex &index,
                               const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fileList.count())
        return false;

    QVector<int> changedRoles;
    switch (role) {
    case ListNameRole:
        fileList[index.row()].listName = value.toString();
        changedRoles << ListNameRole;
    case PemfileRole:
        fileList[index.row()].pemfile = value.toString();
        changedRoles << PemfileRole;
    case PasswordRole:
        fileList[index.row()].password = value.toString();
        changedRoles << PasswordRole;
    case KeyfileRole:
        fileList[index.row()].keyfile = value.toString();
        changedRoles << KeyfileRole;
    default:
        break;
    }

    emit dataChanged(index, index, changedRoles);
    return true;
}

void Chooselist::getDataofList(){
    qDebug() << "#####2######";
    QDomDocument certificatenamelist;
    devman->readhollcertificatelist(certificatenamelist);
    beginResetModel();
   // beginInsertRows(QModelIndex(), rowCount(), rowCount());
    fileList.clear();
    FileInfo fi;
    QDomElement root = certificatenamelist.documentElement(); //根节点
    QDomNode docNode = root.firstChild(); //第一个子节点
    //QString str = docNode.nodeName(); //节点名称


    while(!docNode.isNull()) //遍历文档
    {
        if (docNode.isElement())
        {
            QDomElement e = docNode.toElement();
            QDomNodeList list = e.childNodes(); //子节点的子节点列表(属性列表)
            QDomNode node = list.at(0);
            fi.listName  = node.toElement().text(); //节点内容
            node = list.at(2);
            fi.keyfile = node.toElement().text(); //节点内容
             node = list.at(1);
             fi.pemfile = node.toElement().text();
             node = list.at(3);
             fi.password = node.toElement().text();
             node = list.at(4);
             fi.effectiveflag = node.toElement().text();
             fileList << fi;
             qDebug()<<"listname:"<<fi.listName<<",keyfile:"<<fi.keyfile<<",pemfile:"<<fi.pemfile<<",password:"<<fi.password<<",effectiveflag:"<<fi.effectiveflag;
         }
        docNode = docNode.nextSibling(); //下一个节点
    }
    /*for (int i=0; i<certificatenamelist.size(); i++) {
        fi.listName = certificatenamelist.at(i);
        fileList << fi;
        qDebug()<<"listname:"<<certificatenamelist.at(i)<<"i:"<<i;
    }*/
    //endInsertRows();
    endResetModel();
}
