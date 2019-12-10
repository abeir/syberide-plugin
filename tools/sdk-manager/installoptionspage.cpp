#include "installoptionspage.h"
#include "pdkinstallwizard.h"
#include "constField.h"
#include<QVBoxLayout>
#include<QSettings>
#include<QFile>



InstallOptionsPage::InstallOptionsPage(QWidget *parent) :
 InstallBasePage(parent)
{
    this->initUI();
    this->initConnect();

}


void InstallOptionsPage::initUI()
{

    //setColoredTitle(tr(" %1").arg(productName()));
    //setColoredSubTitle(tr("sdk is the base of target."));
    setColoredTitle(tr("安装套件选择").arg(productName()));
    //setColoredSubTitle(tr("SDK是编译基础，如没有安装过SDK组件则target不可用。所以未安装过PDK中的SDK组件则选“同时安装 SDK 与 Target”项"));
    QVBoxLayout *layout=new QVBoxLayout(this);
     layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setLayout(layout);

     msgLabel=new QLabel(this);
     msgLabel->setWordWrap(true);

     //sdkPathTip=tr("please select install Path");
     sdkPathTip=tr("请选择安装路径");
     QFont font;
     font.setBold(true);
     msgLabel->setFont(font);
     //msgLabel->setText(tr("Install option."));
     msgLabel->setText(tr("请选择安装类型"));
     QWidget *widget=new QWidget(this);
     QVBoxLayout *vLayout = new QVBoxLayout(widget);
    //InstallSDKAndTargetButton=new QRadioButton(tr("Install SDK And Target"),this);
    //onlyInstallTargetButton=new QRadioButton(tr("Only install Target"),this);
    InstallSDKAndTargetButton=new QRadioButton(tr("同时安装工具链与平台编译环境"),this);
    onlyInstallTargetButton=new QRadioButton(tr("仅安装平台编译环境"),this);
    vLayout->addWidget(InstallSDKAndTargetButton);
    vLayout->addWidget(onlyInstallTargetButton);
    InstallSDKAndTargetButton->setChecked(true);
    vLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    layout->addWidget(msgLabel);
    layout->addWidget(widget);

    layout->addItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));

    registerField(bothSDKTarget,InstallSDKAndTargetButton);
    registerField(onlyTarget,onlyInstallTargetButton);

}

void InstallOptionsPage::initConnect()
{
    connect(InstallSDKAndTargetButton,SIGNAL(clicked()),this,SLOT(setBothTip()));
    connect(onlyInstallTargetButton,SIGNAL(clicked()),this,SLOT(setOnlyTargetTip()));
}

void InstallOptionsPage::initializePage()
{
    QSettings setting(QSettings::NativeFormat,QSettings::UserScope,QString(QLatin1String("qtversions")));
    QFile configFile(setting.fileName());
    if(!configFile.exists())
        onlyInstallTargetButton->setEnabled(false);
    else
        onlyInstallTargetButton->setEnabled(true);
}

void InstallOptionsPage::setBothTip()
{
    //sdkPathTip=tr("please select install Path:");
    sdkPathTip=tr("请选择安装路径:");
}

void InstallOptionsPage::setOnlyTargetTip()
{
    //sdkPathTip=tr("please select the sdk installation path(automatically scan path):");
    sdkPathTip=tr("请选择工具链安装路径(自动扫描到如下路径):");
}

InstallOptionsPage::~InstallOptionsPage()
{
        delete msgLabel;
        delete onlyInstallTargetButton;
        delete InstallSDKAndTargetButton;
}
