#include "qpdktargetitemmodel.h"
#include<QDomElement>

QPDKTargetItemModel::QPDKTargetItemModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootNode=NULL;

}


int QPDKTargetItemModel::rowCount(const QModelIndex &parent ) const
{
    if(parent.column()>0)
        return 0;
     Node * parentNode=nodeFromIndex(parent);
    if(!parentNode)
        return 0;
     int count=parentNode->getChildren()->count();

    return count;

}


int QPDKTargetItemModel::columnCount(const QModelIndex &parent ) const
{

        return 1;
}

QModelIndex QPDKTargetItemModel::parent(const QModelIndex &child) const
{
    Node*childNode=nodeFromIndex(child);
    if(!childNode)
        return QModelIndex();
    Node *parentNode=childNode->parentNode();
    if(!parentNode)
        return QModelIndex();

    Node *gradparentNode=parentNode->parentNode();
    if(!gradparentNode)
        return QModelIndex();
    int row=gradparentNode->getChildren()->indexOf(parentNode);

    return createIndex(row,0,parentNode);
}

QModelIndex QPDKTargetItemModel::index(int row, int column, const QModelIndex &parent ) const
 {
    if(!rootNode|| row<0||column<0)
        return QModelIndex();
       Node* parentNode=nodeFromIndex(parent);
       Node* childNode=parentNode->getChildren()->value(row);
       if(!childNode)
           return QModelIndex();
       return createIndex(row,column,childNode);
 }

QVariant QPDKTargetItemModel::data(const QModelIndex &index, int role ) const
{
        if(!index.isValid())
            return QVariant();
        Node *node=nodeFromIndex(index);
        if(!node)
            return QVariant();

        if (role==Qt::CheckStateRole)
       {
            int checkedState=node->isChecked();
            if(CHECKBOX_CHECKED==checkedState)
                return static_cast<int>(Qt::Checked);
            else if(CHECKBOX_UNCHECKED==checkedState)
                return static_cast<int>(Qt::Unchecked);
            else if(CHECKBOX_PARTIALLYCHECKED==checkedState)
                return static_cast<int>(Qt::PartiallyChecked);
       }else if(role==Qt::DisplayRole)
            return node->getInfo();
       return QVariant();
}

void QPDKTargetItemModel::setRootNode(Node *rootNode,QList<QString> initList)
{
    beginResetModel();
    delete this->rootNode;
    this->rootNode=rootNode;
    this->initList=initList;
    setCheckInitState(rootNode);
    endResetModel();
}
QVariant QPDKTargetItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if((orientation==Qt::Horizontal)&&(role==Qt::DisplayRole))
        {
                if(section==0)
                    return tr("Name");
//                else if(section==1)
//                    return tr("Attribute");
        }
        return QVariant();
}

Qt::ItemFlags	QPDKTargetItemModel::flags(const QModelIndex & index) const
{
    if(!index.isValid())
        return 0;
    if(index.column()==0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

}

Node *QPDKTargetItemModel::nodeFromIndex(const QModelIndex &index) const
{

        if(index.isValid()){
            return static_cast<Node*>(index.internalPointer());
        }else
        {
            return rootNode;
        }
}

void QPDKTargetItemModel::childItemChange(const QModelIndex &index,Node *nodeItem, int isChecked)
{
        nodeItem->setCheckState(isChecked);
        int childCount=nodeItem->getChildren()->count();
        for(int i=0;i<childCount;i++)
        {
            Node *childNodeItem = nodeItem->getChildren()->at(i);
            childNodeItem->setCheckState(isChecked);
            QModelIndex childIndex=this->index(i,0,index);
            childItemChange(childIndex,childNodeItem,isChecked);
            emit(dataChanged(index,index));
        }
}

void QPDKTargetItemModel::updateModel(const QModelIndex &index)
{
    Node *nodeItem=nodeFromIndex(index);
    int isChecked=nodeItem->isChecked();
    int newCheckState;
    if((CHECKBOX_UNCHECKED == isChecked) || (CHECKBOX_PARTIALLYCHECKED == isChecked)) newCheckState = CHECKBOX_CHECKED;
    else if(CHECKBOX_CHECKED == isChecked) newCheckState = CHECKBOX_UNCHECKED;

     childItemChange(index,nodeItem,newCheckState);

     parentItemChange(index,nodeItem);

}

void QPDKTargetItemModel::setCheckInitState(Node *rootNode)
{
    if(!rootNode)
        return;

    for(int i=0;i<rootNode->getChildren()->size();i++)
    {
        Node *nodechild=rootNode->getChildren()->at(i);
        QList<QString>::iterator it=std::find(initList.begin(),initList.end(),nodechild->getInfo());
        if(it!=initList.end())
            nodechild->setCheckState(CHECKBOX_CHECKED);
        setCheckInitState(nodechild);
     //  qDebug()<<nodechild->getInfo()<<nodechild->children.size();
    }

}

Node* QPDKTargetItemModel::getRootNode()
{
      return rootNode;
}

void QPDKTargetItemModel::parentItemChange(const QModelIndex &index, Node *nodeItem)
{
    Node *parentNodeItem = nodeItem->parentNode();
    if(NULL == parentNodeItem)  return;


    int selectedCount = 0;
    int partiallySelectedCount = 0;
    int childCount = parentNodeItem->getChildren()->size();
    for(int i=0;i<childCount;i++)
    {
        Node *childNodeItem = parentNodeItem->getChildren()->at(i);
        int checkedState = childNodeItem->isChecked();
        if(CHECKBOX_CHECKED==checkedState)
            selectedCount++;
        else if(CHECKBOX_PARTIALLYCHECKED==checkedState)
                    partiallySelectedCount++;
    }

    if(selectedCount == childCount)
    {
        parentNodeItem->setCheckState(CHECKBOX_CHECKED);
    }
    else if(0 == selectedCount && 0 == partiallySelectedCount)
    {
        parentNodeItem->setCheckState(CHECKBOX_UNCHECKED);
    }
    else//部分选中
    {
        parentNodeItem->setCheckState(CHECKBOX_PARTIALLYCHECKED);
    }

    emit(dataChanged(index, index));
    //递归调用
    QModelIndex parentIndex = this->parent(index);
    parentItemChange(parentIndex,parentNodeItem);
}


