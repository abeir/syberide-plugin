
/*
* This file is part of SyberSDK
*
* Copyright (C) 2014 Beijing Yuan Xin Technology Co.,Ltd. All rights reserved.
*
* Authors:
*       Wang pei <wangpei@syberos.com>
* This software, including documentation, is protected by copyright controlled
* by Beijing Yuan Xin Technology Co.,Ltd. All rights are reserved.
*/

#include "objectlistmodel.h"
#include <QDebug>



ObjectListModel::ObjectListModel(QObject *parent, QList<QObject*> *list)
    : QAbstractListModel(parent),
      _list(list)
{
    //! modify
//    QHash<int, QByteArray> roles;
//    roles[Qt::UserRole + 1] = "object";
//    setRoleNames(roles);
    //! --

    _roleNames[Qt::UserRole + 1] = "object";
}

int ObjectListModel::indexOf(QObject *obj) const
{
    return _list->indexOf(obj);
}

int ObjectListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list->count();
}

int ObjectListModel::itemCount() const
{
    return _list->count();
}

int ObjectListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _list->count())
        return QVariant();

    if (role == Qt::UserRole + 1)
    {
        QObject *obj = _list->at(index.row());
        return QVariant::fromValue(obj);
    }

    return QVariant(0);
}

bool ObjectListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= _list->count())
        return false;

    if (role == Qt::UserRole + 1)
    {
        _list->replace(index.row(), reinterpret_cast<QObject*>(value.toInt()));
        return true;
    }

    return false;
}

void ObjectListModel::insertItem(int index, QObject *item)
{
    beginInsertRows(QModelIndex(), index, index);
    _list->insert(index, item);
    connect(item, SIGNAL(destroyed()), this, SLOT(removeDestroyedItem()));
    endInsertRows();

    emit itemAdded(item);
    emit itemCountChanged();
}

void ObjectListModel::addItem(QObject *item)
{
    insertItem(_list->count(), item);
}


void ObjectListModel::removeDestroyedItem()
{
    QObject *obj = QObject::sender();
    removeItem(obj);
}

void ObjectListModel::removeItem(QObject *item)
{
    int index = _list->indexOf(item);
    if (index >= 0) {
        beginRemoveRows(QModelIndex(), index, index);
        _list->removeAt(index);
        disconnect(item, SIGNAL(destroyed()), this, SLOT(removeDestroyedItem()));
        endRemoveRows();
        emit itemCountChanged();
    }
}

//! modify
QObject * ObjectListModel::takeItemAt(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    disconnect(((QObject*)_list->at(index)), SIGNAL(destroyed()), this, SLOT(removeDestroyedItem()));
    QObject * item = _list->takeAt(index);
    endRemoveRows();
    emit itemCountChanged();

    return item;
}
//! --

void ObjectListModel::removeItem(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    disconnect(((QObject*)_list->at(index)), SIGNAL(destroyed()), this, SLOT(removeDestroyedItem()));
    _list->removeAt(index);
    endRemoveRows();
    emit itemCountChanged();
}

QObject* ObjectListModel::get(int index)
{
    if (index >= _list->count() || index < 0)
        return 0;

    return _list->at(index);
}

QList<QObject*> *ObjectListModel::getList()
{
    return _list;
}

void ObjectListModel::setList(QList<QObject *> *list)
{
    QList<QObject *> *oldList = _list;
    beginResetModel();
    _list = list;
    endResetModel();
    emit itemCountChanged();
    delete oldList;
}

void ObjectListModel::reset()
{
    //! modify
//    QAbstractListModel::reset();

    beginResetModel();
    qDeleteAll(_list->begin(), _list->end()); //add by fh
    _list->clear();
    endResetModel();
    //! --

    emit itemCountChanged();
}

void ObjectListModel::move(int oldRow, int newRow)
{
    //! modify
    if (oldRow == newRow)
        return;
    //! --

    if (oldRow < 0 || oldRow >= _list->count())
        return;

    if (newRow < 0 || newRow >= _list->count())
        return;

    beginMoveRows(QModelIndex(), oldRow, oldRow, QModelIndex(), (newRow > oldRow) ? (newRow + 1) : newRow);
    _list->move(oldRow, newRow);
    endMoveRows();
}

QHash<int, QByteArray>	ObjectListModel::roleNames() const
{
    return _roleNames;
}
