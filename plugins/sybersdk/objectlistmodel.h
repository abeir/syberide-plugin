/*
* This file is part of cmos-weather
*
* Copyright (C) 2014 Beijing Yuan Xin Technology Co.,Ltd. All rights reserved.
*
* Authors:
*       Wang pei <wangpei@syberos.com>
* This software, including documentation, is protected by copyright controlled
* by Beijing Yuan Xin Technology Co.,Ltd. All rights are reserved.
*/

#ifndef OBJECTLISTMODEL_H
#define OBJECTLISTMODEL_H

#include <QAbstractListModel>


class ObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)

    QList<QObject*> *_list;
    //! modify
    QHash<int, QByteArray> _roleNames;

public:
    explicit ObjectListModel(QObject *parent = 0, QList<QObject*> *list = new QList<QObject*>());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int itemCount() const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void move(int oldRow, int newRow);

    void insertItem(int index, QObject *item);

    void addItem(QObject *item);
    void removeItem(QObject *item);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE QObject* get(int index);
    int indexOf(QObject *obj) const;

   //  void replaceItem(int itemIndex, QObject *item);

    //! modify
    QObject * takeItemAt(int index);
    //! --

    template<typename T>
    QList<T*> *getList();
    QList<QObject*> *getList();

    template<typename T>
    void setList(QList<T*> *list);
    void setList(QList<QObject*> *list);

    //! modify
    virtual QHash<int, QByteArray>	roleNames() const;
    //! --

private slots:
    void removeDestroyedItem();

signals:
    void itemAdded(QObject *item);
    void itemCountChanged();
};

template<typename T>
QList<T*> *ObjectListModel::getList()
{
    return reinterpret_cast<QList<T *> *>(_list);
}

template<typename T>
void ObjectListModel::setList(QList<T*> *list)
{
    setList(reinterpret_cast<QList<QObject *> *>(list));
}

//}
//}
#endif // OBJECTLISTMODEL_H
