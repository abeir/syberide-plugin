#ifndef QPDKTARGETITEMMODEL_H
#define QPDKTARGETITEMMODEL_H

#include <QAbstractItemModel>
#include "parserfile.h"
#include "QDomElement"
#include "node.h"

class QPDKTargetItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit QPDKTargetItemModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setRootNode(Node *rootNode,QList<QString> initList);

    Node* nodeFromIndex(const QModelIndex &index) const;

    Qt::ItemFlags	flags(const QModelIndex & index) const;
   // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//        int role = Qt::EditRole);
    void updateModel(const QModelIndex &index);
    Node* getRootNode();


signals:

public slots:

private:
    void setCheckInitState(Node * rootNode);
    void parentItemChange(const QModelIndex &index,Node *nodeItem);
    void childItemChange(const QModelIndex &index,Node *nodeItem, int isChecked);
    Node* rootNode;
    QList<QString> initList;
    int flag;


};

#endif // QPDKTARGETITEMMODEL_H
