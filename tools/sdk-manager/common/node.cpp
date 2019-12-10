#include "node.h"

Node::Node(const QString &nodeStr)
{
    this->info=nodeStr;\
    m_checked=CHECKBOX_UNCHECKED;
    parent=0;
}

Node::~Node()
{
    qDeleteAll(children);
}

QString Node::getInfo()
{
        return info;
}

void Node::setInfo(const QString &nodeInfo)
{
        this->info=nodeInfo;
}

QList<Node*> *Node::getChildren()
{
        return &children;
}

void Node::setParentNode(Node *parent)
{
    this->parent=parent;
}

Node * Node::parentNode()
{
    return parent;
}
