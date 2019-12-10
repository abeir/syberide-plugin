#include "parserfile.h"
#include<QDomElement>
#include<QDomDocument>
#include<QDomNodeList>
#include<QDebug>
#include<QFile>


static const QLatin1String SDKName("SDKName");
static const QLatin1String Targets("Targets");
static const QLatin1String Name("Name");
static const QLatin1String archName("archName");
static const QLatin1String Arch("arch");
static const QLatin1String BranchNames("branchNames");

ParserFile::ParserFile()
{

}

Node* ParserFile::ParserXmlFile(const QString &pathFileName)
{
        QFile file(pathFileName);

        QDomDocument document;
        QDomElement rootElement=getRootElementXmlFile(&file,&document,QIODevice::ReadOnly);
        if(rootElement.isNull())
            return NULL;
        Node *rootNode=new Node(rootElement.tagName());
        QDomNodeList childNodes=rootElement.childNodes();
        for(int i=0;i<childNodes.count();i++)
        {
            const QDomElement childE=childNodes.at(i).toElement();
            if(childE.isNull())
                continue;
            if(childE.tagName()==SDKName){
                Node* sdkNode=new Node(childE.text());
                addChild(rootNode,sdkNode);
            }else if(childE.tagName()==Targets){
              for(int i=0;i<childE.childNodes().count();i++)
              {
                  const QDomElement childTarget=childE.childNodes().at(i).toElement();
                  if(childTarget.isNull())
                      continue;
                  if(childTarget.tagName()==Name){
                            Node* targetNode=new Node(childTarget.text());
                            addChild(rootNode,targetNode);
                            parseXmlElement(targetNode,childE);
                            break;
                  }
              }
            }
        }

       return rootNode;

}

QDomElement ParserFile::getRootElementXmlFile(QFile *file, QDomDocument *doc, QIODevice::OpenMode mode)
{
           if(!file->exists()){
               qDebug()<<file<<" is not exist!";
               return QDomElement();
           }
           if(!file->open(mode))
           {
               qDebug() <<"Cannot open file for reading:"
                        <<qPrintable(file->errorString());
               return QDomElement();
           }
           QString strError;
           int  errLin = 0, errCol = 0;
           if( !doc->setContent(file,  &strError, &errLin, &errCol) ) {
                  qDebug()<<"read xml error"<<errLin<<errCol<<strError;
                  return QDomElement();
           }

           if( doc->isNull() ) {
               qDebug()<< "document is null !\n" ;
               return QDomElement();
           }

           QDomElement root = doc->documentElement();
           return root;

}

void ParserFile::parseXmlElement(Node *parent, const QDomElement &element)
{
    if(element.isNull())
        return;
    for(int i=0;i<element.childNodes().count();i++)
    {

            QDomElement childE=element.childNodes().at(i).toElement();

            if(childE.tagName()==Arch)
            {

                for(int i=0;i<childE.childNodes().count();i++)
                {
                    QDomElement archChildE=childE.childNodes().at(i).toElement();
                     if(archChildE.tagName()==archName)
                    {
                         Node* archNode=new Node(archChildE.text());
                        addChild(parent,archNode);
                        for(int i=0;i<childE.childNodes().count();i++)
                        {

                            QDomElement branchChildE=childE.childNodes().at(i).toElement();
                            if(branchChildE.tagName()==BranchNames)
                            {
                                            for(int i=0;i<branchChildE.childNodes().count();i++)
                                            {
                                                QDomElement branchNodeE=branchChildE.childNodes().at(i).toElement();
                                                Node* branchNode=new Node(branchNodeE.text());
                                                addChild(archNode,branchNode);
                                            }
                            }
                        }
                    }
                }

            }
    }

}

void ParserFile::printf(Node *node)
{

         if(node->getInfo().isEmpty())
             return;
            qDebug()<<"****"<<node->getInfo()<<endl;

            for(int i=0;i<node->getChildren()->size();i++)
            {
                Node *nodechild=node->getChildren()->at(i);
                printf(nodechild);

            }
}

void ParserFile::addChild(Node *parent, Node *child)
{
    if(child){
      parent->getChildren()->append(child);
      child->setParentNode(parent);
    }
}

