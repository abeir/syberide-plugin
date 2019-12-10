#include "pdkinstallwizard.h"
#include "installmodeselectpage.h"
#include "installoptionspage.h"
#include "selectsdkinstallpathpage.h"
#include "localinstallselecttargetpage.h"
#include "viewinstalledinfopage.h"
//#include "performinstallpage.h"

#include "installfinishpage.h"


PDKInstallWizard::PDKInstallWizard(QWidget *parent) :
    QWizard(parent)
{
    this->setWindowTitle(tr("交叉编译套件管理工具"));//this->setWindowTitle(QString(QLatin1String("PDK-manager")));
    this->setWizardStyle(QWizard::ModernStyle);
    this->setWindowIcon(QIcon(QString(QLatin1String(":/icon/banner"))));

    this->currentPageID=-1;
    this->setOption(QWizard::NoBackButtonOnStartPage,true);
    this->setOption(QWizard::NoBackButtonOnLastPage,true);
    this->setOption(QWizard::NoCancelButtonOnLastPage,true);
    this->setButtonText(QWizard::NextButton,tr("下一步"));//this->setButtonText(QWizard::NextButton,tr("Next"));
    this->setButtonText(QWizard::CommitButton,tr("安装"));//this->setButtonText(QWizard::CommitButton,tr("Install"));
    this->setButtonText(QWizard::BackButton,tr("上一步"));//this->setButtonText(QWizard::BackButton,tr("back"));
    this->setButtonText(QWizard::CancelButton,tr("取消"));//this->setButtonText(QWizard::CancelButton,tr("cancel"));
    this->setButtonText(QWizard::FinishButton,tr("结束"));//this->setButtonText(QWizard::FinishButton,tr("finish"));

    performInstallPage=new PerformInstallPage();
    setPage(InstallModePageID,new InstallModeSelectPage());
    setPage(ViewInstalledInfoPageID,new viewInstalledInfoPage());
    setPage(InstallOptionsPageID,new InstallOptionsPage());
    setPage(SelectSDKPathPageID,new SelectSDKInstallPathPage());
    setPage(SelectTargetInstallPageID,new SelectTargetInstallPage());
    setPage(LocalInstallSelectTargetPageID,new localinstallselecttargetpage());
    setPage(PerformInstallPageID,performInstallPage);
    setPage(InstallationFinishedPageID,new InstallFinishPage());



    connect(this,SIGNAL(currentIdChanged(int)),this,SLOT(performCorrespondPage(int)));
    connect(performInstallPage,SIGNAL(installFinished()),this,SLOT(showFinishedPage()));
    //this->setFixedWidth(550);

}

void PDKInstallWizard::performCorrespondPage(int id)
{
   InstallBasePage *oldPage=qobject_cast<InstallBasePage *>(page(this->currentPageID));
   if(oldPage){
       oldPage->leaving();
   }

   this->currentPageID=id;
   InstallBasePage *newPage=qobject_cast<InstallBasePage*>(page(this->currentPageID));
   if(newPage){
       newPage->entering();
   }
}

void PDKInstallWizard::showFinishedPage()
{
        next();
}








