#include "localinstallselecttargetpage.h"
#include "constField.h"
#include<QHBoxLayout>
#include<QFileDialog>
#include<QDebug>
#include<QMessageBox>

localinstallselecttargetpage::localinstallselecttargetpage(QWidget *parent):
InstallBasePage(parent)
{
    this->initUI();
    this->initConnect();

}

void localinstallselecttargetpage::initUI()
{
    setColoredTitle(tr(" %1").arg(productName()));
    setColoredSubTitle(QString(QLatin1String("")));
    setPixmap(QWizard::WatermarkPixmap,QPixmap());

    sdkpackageTip=new QLabel;
    targetpackageTip=new QLabel;
    targetpackgePathLineEdit=new QLineEdit();
    targetpackgePathLineEdit->setFixedWidth(350);
    targetpackgePathLineEdit->setReadOnly(true);
    targetpackageBrowseButton=new QPushButton(tr("Browse"));

    sdkpackagePathLineEdit=new QLineEdit();
    sdkpackagePathLineEdit->setFixedWidth(350);
    sdkpackagePathLineEdit->setReadOnly(true);
    sdkpackageBrowseButton=new QPushButton(tr("Browse"));

    sdkpackageTip->setText(tr("请选择交叉编译工具链安装包"));//sdkpackageTip->setText(tr("please select sdk install package"));
    targetpackageTip->setText(tr("请选择平台编译环境安装包,可多选"));//targetpackageTip->setText(tr("please select target install package,it can select multiple targets."));

    QHBoxLayout *hLayout=new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(sdkpackagePathLineEdit);
    hLayout->addWidget(sdkpackageBrowseButton);
    hLayout->addItem(new QSpacerItem(60, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *targetpackageLayout=new QHBoxLayout;
    targetpackageLayout->addStretch();
    targetpackageLayout->addWidget(targetpackgePathLineEdit);
    targetpackageLayout->addWidget(targetpackageBrowseButton);
    targetpackageLayout->addItem(new QSpacerItem(60, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    targetpackageLayout->setContentsMargins(0,0,0,0);

      QVBoxLayout *vLayout=new QVBoxLayout;
      vLayout->addWidget(sdkpackageTip);
      vLayout->addLayout(hLayout);
      vLayout->addWidget(targetpackageTip);
      vLayout->addLayout(targetpackageLayout);
      vLayout->addItem(new QSpacerItem(10, 150, QSizePolicy::Minimum, QSizePolicy::Expanding));
      this->setLayout(vLayout);

      setCommitPage(true);

      registerField(sdkpackagePath,sdkpackagePathLineEdit);
      registerField(targetpackgePath,targetpackgePathLineEdit);

}

void localinstallselecttargetpage::initializePage()
{
        bool modeoption=field(bothSDKTarget).toBool();
        if(modeoption)
        {
            sdkpackageTip->show();
            sdkpackagePathLineEdit->show();
            sdkpackageBrowseButton->show();
           setColoredSubTitle(tr("sdk install package and target install package is local compress  tar package."));
        }else
        {
            sdkpackageTip->hide();
            sdkpackagePathLineEdit->hide();
            sdkpackageBrowseButton->hide();
            setColoredSubTitle(tr("target install package is local compress tar spackage."));
        }
}

void localinstallselecttargetpage::selectSDKInstallPackages()
{
        sdkfileName=QFileDialog::getOpenFileName(this,tr("select sdk install  package"),QDir::homePath(),QString(QLatin1String("Text files(*.tar.bz2)")));
        sdkpackagePathLineEdit->setText(sdkfileName);
}
void localinstallselecttargetpage::selectTargetInstallPackages()
{
        QString targetPackageNames=selectpackages(tr("select target install  package"));

        targetpackgePathLineEdit->setText(targetPackageNames);

}

QString localinstallselecttargetpage::selectpackages(QString tiptext)
{
       targetfileNames=QFileDialog::getOpenFileNames(this,tiptext,QDir::homePath(),QLatin1String("Text files(*.tar.bz2)"));
        QString packageNames;
        foreach (QString fileName, targetfileNames) {
            packageNames+=fileName;
            packageNames+=QString(QLatin1String(";"));
        }
        return packageNames;
}

void localinstallselecttargetpage::initConnect()
{
     connect(sdkpackageBrowseButton,SIGNAL(clicked()),this,SLOT(selectSDKInstallPackages()));
     connect(targetpackageBrowseButton,SIGNAL(clicked()),this,SLOT(selectTargetInstallPackages()));
}

bool localinstallselecttargetpage::validatePage()
{
        bool modeoption=field(bothSDKTarget).toBool();
        if(modeoption)
        {
            if(sdkpackagePathLineEdit->text().isEmpty())
            {
                QMessageBox::warning(0, QMessageBox::tr("The select is empty!"), QMessageBox::tr("The  sdk package is empty! "));
                return false;
            }
            if(!sdkfileName.contains(QString(QLatin1String("syberos_sdk"))))
            {
                QMessageBox::warning(0, QMessageBox::tr("The select is error!"), QMessageBox::tr("The selected sdk package is wrong,the sdk package must be start with sysberos_sdk tar package! "));
                return false;
            }
        }
        if(targetpackgePathLineEdit->text().isEmpty())
        {
             QMessageBox::warning(0, QMessageBox::tr("The select is empty!"), QMessageBox::tr("The  target package is empty! "));
             return false;
        }

        foreach (QString fileName, targetfileNames) {
             if (!fileName.contains(QString(QLatin1String("target-"))))
             {
                     QMessageBox::warning(0, QMessageBox::tr("The select is error!"), QMessageBox::tr("The selected target package is wrong,the target package must be start with target- tar package! "));
                     return false;
             }
        }
        return true;

}

