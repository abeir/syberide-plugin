#ifndef NODE_H
#define NODE_H
#include<QList>
#include<QString>

#define CHECKBOX_UNCHECKED     (0)
#define CHECKBOX_CHECKED     (1)
#define CHECKBOX_PARTIALLYCHECKED    (2)
class Node
{
public:
    Node(const QString &nodeStr);
    QString getInfo();
    void setInfo(const QString &nodeInfo);
    Node *parentNode();
    void setParentNode(Node* parent);
    QList<Node*> *getChildren();
    int isChecked(){return m_checked;}
    void setCheckState(int state){m_checked = state;}
   ~Node();
 //  enum Type{targetName,branchName};

private:

   QString info;
   QList<Node*> children;
    Node *parent;
    int m_checked;

};

#endif // NODE_H
