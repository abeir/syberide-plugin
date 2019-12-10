#include "selecttargetinstallpage.h"
#include "installbasepage.h"
#include "pdkinstallwizard.h"
#include "common/parserfile.h"
#include "common/node.h"
#include "common/downloader.h"
#include "constField.h"

#include<QFile>
#include<QDomElement>
#include<QVBoxLayout>
#include<QWizard>
#include<QDir>
#include<QMessageBox>
#include<QJsonDocument>


static const QString armv7tnhl(QLatin1String("armv7tnhl"));
static const QString i686(QLatin1String("i686"));
static const QString PDKTargetInfoFileName(QLatin1String("PdkTargetsInfo.xml"));
static const QString repo(QLatin1String("http://sdk.insyber.com/Releases_Syberos_Pdk/"));
static const QString repofileconfig(QLatin1String("repo.json"));

SelectTargetInstallPage::SelectTargetInstallPage(QWidget *parent):
InstallBasePage(parent)
{

    this->initUI();
    this->initConnect();
}

void SelectTargetInstallPage::initUI()
{
    setColoredTitle(tr(" %1").arg(productName()));
    //setColoredSubTitle(tr("target part into i686 and armv7tnhl.it have multiple branch.for examle:main_dev,xuanwu."));
    setPixmap(QWizard::WatermarkPixmap,QPixmap());

    QVBoxLayout *layout=new QVBoxLayout(this);
     layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setLayout(layout);


    treePDKModel=new QPDKTargetItemModel(this);
    treeView=new QTreeView(this);
    treeView->setModel(treePDKModel);



   m_msgLabel = new QLabel(this);
   m_msgLabel->setWordWrap(true);
   QFont font;
   font.setBold(true);
   m_msgLabel->setFont(font);
   m_msgLabel->setText(tr("请选择平台编译环境版本"));//m_msgLabel->setText(tr("please select i686 or armv7tnhl  the target"));

   QWidget *widget = new QWidget(this);
   QVBoxLayout *vLayout = new QVBoxLayout(widget);
   treeView->setFixedHeight(180);
    vLayout->addWidget(treeView);

   layout->addWidget(m_msgLabel);
   layout->addWidget(widget);

   layout->addItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setCommitPage(true);

}

void SelectTargetInstallPage::printf(Node *node)
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

void SelectTargetInstallPage::initializePage()
{

    this->setButtonText(QWizard::NextButton,QString(QLatin1String("Install")));

    QString fileName;
    fileName=QDir::tempPath()+QString(QLatin1String("/"))+PDKTargetInfoFileName;
    downloader.setDestFileName(fileName);

    QString reposit=repo;

    if(parseRepojson())
    {
        reposit=this->repoMap.value(QString(QLatin1String("repo"))).toString();
    }


    QNetworkReply *reply=downloader.startDownload(reposit,PDKTargetInfoFileName);
    Q_UNUSED(reply);
}

void SelectTargetInstallPage::updateModel(const QModelIndex &index)
{
        treePDKModel->updateModel(index);
        treeView->collapse(index);
        treeView->expand(index);

}

bool SelectTargetInstallPage::parseRepojson()
{
    QFile file(repofileconfig);
    if(!file.exists())
        return false;

    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return false;
    }
    QJsonDocument jsonDocument=QJsonDocument::fromJson(file.readAll());
    if(jsonDocument.isNull())
    {   file.close();
        return false;
    }
    repoMap=jsonDocument.toVariant().toMap();

    return true;

}

int SelectTargetInstallPage::nextId() const
{
     return PDKInstallWizard::PerformInstallPageID;
}

void SelectTargetInstallPage::getTraversalData(Node *node)
{
    if(node->getInfo().isEmpty())
        return;
    for(int i=0;i<node->getChildren()->size();i++)
    {
        Node *nodechild=node->getChildren()->at(i);
        Node *branchNode=NULL;
        if(nodechild->getInfo()==armv7tnhl)
         {
            for (int j=0;j<nodechild->getChildren()->size();j++) {
                branchNode=nodechild->getChildren()->at(j);
                if(branchNode->isChecked())
                    selectionData.insert(armv7tnhl,branchNode->getInfo());
            }
        }else if(nodechild->getInfo()==i686)
        {
            for (int j=0;j<nodechild->getChildren()->size();j++) {
                branchNode=nodechild->getChildren()->at(j);
                if(branchNode->isChecked())
                    selectionData.insert(i686,branchNode->getInfo());
            }
        }else
             getTraversalData(nodechild);

    }

}


bool SelectTargetInstallPage::getSelectData()
{
    selectionData.clear();
    Node *rootNode=treePDKModel->getRootNode();
    if(!rootNode)
        return false;
    getTraversalData(rootNode);
    return true;
}

bool SelectTargetInstallPage::validatePage()
{
    bool result=getSelectData();
    return result;
}

void SelectTargetInstallPage::initModel()
{
    QString fileName;
    fileName=QDir::tempPath()+QString(QLatin1String("/"))+PDKTargetInfoFileName;

    Node* rootNode=ParserFile::ParserXmlFile(fileName);
    QList<QString> initList;
    initList<<QString(QLatin1String("main_dev"));
    if(rootNode)
        treePDKModel->setRootNode(rootNode,initList);

    treeView->expandAll();
}

void SelectTargetInstallPage::initConnect()
{
    connect(treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(updateModel(QModelIndex)));
    connect(&downloader,SIGNAL(downfinished()),this,SLOT(initModel()));
}
