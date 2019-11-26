#include "versionwidget.h"
#include "ui_versionwidget.h"
#include "licensewidget.h"
#include <QSettings>
#include <QTextCodec>
#include <QPixmap>
#include <QFile>
#include <QCoreApplication>
#include <QDomDocument>
#include <QMultiMap>

VersionWidget::VersionWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("关于SyberOS SDK"));
    this->setFixedSize(573,  353);
    connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(close()));


    QFont font = ui->versionLabel->font();
    font.setBold(true);
    ui->versionLabel->setFont(font);

    QString strExePath = QCoreApplication::applicationDirPath();
    int nPos = strExePath.lastIndexOf(QLatin1String("/"));
    QString strDir = strExePath.mid(0, nPos) + QLatin1String("/sdkversion/version.ini");
    QSettings settings(strDir, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup(tr("version"));
  //  QString strMaxversion = settings.value(tr("versioninfo"), tr("版本号: 2.0.0.1")).toString();
    QString strBuildTime = settings.value(tr("buildtime"), tr("构建时间：2015-09-13 13:25:23")).toString();
    QString strCopyright = settings.value(tr("copyright"), tr("Copyright 2013-2015 Yuan Xin All rights reserved")).toString();
    settings.endGroup();

    packageInfo();

    QString strPackage=tr("版本号:"); //= tr("名称                                         版本\n");
     strPackage += map.value(tr("ApplicationVersion"));



     QString ComponentInfo=tr("组件名称:");

    ui->versionLabel->setText(strPackage);
    ui->buildTimeLabel->setText(strBuildTime);
    ui->copyrightLabel->setText(strCopyright);
    ui->CompNameLabel->setText(ComponentInfo);

    QStringList compNameList=map.values(tr("componentName"));
    int flag=0;
    foreach (QString compName, compNameList) {
           if(flag==0)
           {
               flag=1;
               ui->CompNameEditDisplay->setText(compName);
           }
           else
           {
               ui->CompNameEditDisplay->append(tr(""));
               ui->CompNameEditDisplay->append(compName);
           }
    }
    ui->CompNameEditDisplay->setReadOnly(true);
    ui->CompNameEditDisplay->setStyleSheet(tr("background-color: #fffaf0;"));

   //ui->packageLabel->setText(strPackage);
 // ui->packageLabel->adjustSize();
 //   ui->appVersionLabel->setText(strPackage);

    ui->imagelabel->setPixmap(QPixmap(tr(":/icon/version.png")));

    connect(ui->copyrightBtn, SIGNAL(clicked()), this, SLOT(slotShowCopyright()));
}

VersionWidget::~VersionWidget()
{
    delete ui;
}

void VersionWidget::slotShowCopyright()
{
    LicenseWidget widget;
    widget.exec();
}

void VersionWidget::packageInfo()
{
    QString strExePath = QCoreApplication::applicationDirPath();
    int nPos = strExePath.lastIndexOf(QLatin1String("/"));
    strExePath = strExePath.mid(0, nPos);
    nPos = strExePath.lastIndexOf(QLatin1String("/"));
    QString strPath = strExePath.mid(0, nPos) + QLatin1String("/components.xml");

    QFile file(strPath);
     if(!file.open(QFile::ReadOnly | QFile::Text))
         return ;

      QDomDocument document;
      QString strError;
      int nRow,nColumn;
      if(!document.setContent(&file, false, &strError, &nRow, &nColumn))
      {
          file.close();
          return;
      }
      if(document.isNull())
          return ;
      QDomElement root = document.documentElement();
      QDomNodeList packgeNodes = root.childNodes();
      QString strResult;
      for(int i = 0; i<packgeNodes.count(); ++i){
          if(packgeNodes.at(i).toElement().tagName()==tr("ApplicationVersion"))
          {
              map.insert(tr("ApplicationVersion"),packgeNodes.at(i).toElement().text());
          }

          if(packgeNodes.at(i).toElement().tagName().compare(tr("Package")) != 0)
              continue;
          map.insert(tr("componentName"),packgeNodes.at(i).firstChildElement(tr("Name")).toElement().text());
//           strResult += packgeNodes.at(i).firstChildElement(tr("Name")).toElement().text();
//           strResult += tr("                   ");
//           strResult += packgeNodes.at(i).firstChildElement(tr("Version")).toElement().text();
//           strResult += tr("\n");
      }
      file.close();
 //     return strResult;
}
