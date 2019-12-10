#include "viewinstalledinfopage.h"
#include<QLabel>
#include<QVBoxLayout>
#include<QSettings>
#include<QFile>
#include<QTextStream>
#include<QStringList>
#include<QHeaderView>

viewInstalledInfoPage::viewInstalledInfoPage(QWidget *parent ):
 InstallBasePage(parent)
{
    this->initUI();
}

void viewInstalledInfoPage::initUI()
{
    setColoredTitle(tr(" %1").arg(productName()));
    setColoredSubTitle(tr("PDK is Platform SDK,PDK contain one sdk and multiple target."));
    setPixmap(QWizard::WatermarkPixmap,QPixmap());

    m_msgLabel = new QLabel(this);
    m_msgLabel->setWordWrap(true);
    QFont font;
    font.setBold(true);
    m_msgLabel->setFont(font);
    m_msgLabel->setText(tr("the current system,PDK has been install the SDK and target information."));

    installedInfoTableWidget=new QTableWidget;
    installedInfoTableWidget->setFixedHeight(180);
    installedInfoTableWidget->setColumnCount(4);
    installedInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    headerView=new QHeaderView(Qt::Horizontal);
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    QStringList headerLabels;
    headerLabels<<tr("types")<<tr("arch")<<tr("branchName")<<tr("status");
    installedInfoTableWidget->setHorizontalHeader(headerView);
    installedInfoTableWidget->setHorizontalHeaderLabels(headerLabels);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_msgLabel);
    vLayout->addSpacing(20);
    vLayout->addWidget(installedInfoTableWidget);
    vLayout->addItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->setLayout(vLayout);

}

void viewInstalledInfoPage::initializePage()
{
    setComplete(false);
    installedInfoTableWidget->clearContents();
    int len=installedInfoTableWidget->rowCount();
    for(int i=0;i<len;i++)
            installedInfoTableWidget->removeRow(0);
    QSettings setting(QSettings::NativeFormat,QSettings::UserScope,QString(QLatin1String("qtversions")));
    parseConfigFile(setting.fileName());

}

void viewInstalledInfoPage::addrowForTableWidget(const QString &name,const QString &arch,const QString & branchName)
{
    int rowcount=installedInfoTableWidget->rowCount();
    QTableWidgetItem *nameItem=new QTableWidgetItem(name);
    nameItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QTableWidgetItem *archItem=new QTableWidgetItem(arch);
    archItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QTableWidgetItem *branchItem=new QTableWidgetItem(branchName);
    branchItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
   QTableWidgetItem *statusItem=new QTableWidgetItem(tr("installed"));
    statusItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    installedInfoTableWidget->insertRow(rowcount);
    installedInfoTableWidget->setItem(rowcount,0,nameItem);
    installedInfoTableWidget->setItem(rowcount,1,archItem);
    installedInfoTableWidget->setItem(rowcount,2,branchItem);
    installedInfoTableWidget->setItem(rowcount,3,statusItem);
}

bool viewInstalledInfoPage::parseConfigFile(QString configFileName)
{
    QFile configFile(configFileName);
    if(configFile.exists())
    {
        if(!configFile.open(QIODevice::ReadOnly|QIODevice::Text))
            return false;
        QTextStream in(&configFile);
        while(!in.atEnd())
        {
            QString line=in.readLine();
            if(!line.isEmpty())
            {
               QStringList listData=line.split(QString(QLatin1String("=")));
               QFile file(listData.at(1));
               if(file.exists())
               {
                   if(listData.at(0).contains(QString(QLatin1String("sdk-install"))))
                   {
                      addrowForTableWidget(tr("sdk"),tr("not distinguish"),tr("not distinguish"));

                   }else if(listData.at(0).contains(QString(QLatin1String("target-"))))
                   {
                        QStringList targetinfoList=listData.at(0).split(QString(QLatin1String("-")));
                        addrowForTableWidget(targetinfoList.at(0),targetinfoList.at(1),targetinfoList.at(2));
                   }
               }
            }
        }
        configFile.close();
    }

    return true;
}

