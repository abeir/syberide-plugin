#ifndef PARSERFILE_H
#define PARSERFILE_H

#include<QDomElement>
#include<QDomDocument>
#include<QFile>
#include "node.h"

class ParserFile
{
public:
    ParserFile();
    static Node* ParserXmlFile(const QString &pathFileName);

protected:
    static QDomElement  getRootElementXmlFile(QFile *file,QDomDocument *doc,QIODevice::OpenMode mode);
    static void parseXmlElement(Node*parent,const QDomElement &element);
    static void addChild(Node *parent,Node *child);
    static void printf(Node *node);


 private:
//     QDomDocument document;
//     QDomElement root;
// Node *node;

};

#endif // PARSERFILE_H
