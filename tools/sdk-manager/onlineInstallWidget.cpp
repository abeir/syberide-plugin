#include"onlineInstallWidget.h"
#include<QLineEdit>
#include<QWidget>
#include<QDir>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QFormLayout>
#include<QDialogButtonBox>
#include<QDebug>

const char sdkInstallPathTip[] = "工具链安装路径:";
const char sdkInstallPath[] = "/SyberOS-PDK";//const char sdkInstallPath[] = "/Syberos-PDK";
const char onInstallSDKTip[] = "只安装工具链";
const char onInstallTargetTip[] = "只安装平台编译环境";
const char installSDKAndTargetTip[] = "同时安装工具链与平台编译环境";
const char platformArchTip[] = "平台架构:";

OnlineInstallWidget::OnlineInstallWidget(QWidget *parent):
QWidget(parent)
{
    this->initUI();
    this->initConnect();
}

void OnlineInstallWidget::createComponent()
{
    sdkPathTextLabel=new QLabel;
    sdkPathTextLabel->setText(QString(sdkInstallPathTip));
   sdkPathLineEdit=new QLineEdit;
   sdkPathLineEdit->setText(QString(QDir::homePath()+QString(sdkInstallPath)));
   sdkPathLineEdit->setFixedWidth(280);
   onlyInstallSDKButton=new QRadioButton;
   onlyInstallSDKButton->setText(QString(onInstallSDKTip));

   onlyInstallTargetButton=new QRadioButton;
   onlyInstallTargetButton->setText(QString(onInstallTargetTip));

  InstallButton=new QRadioButton;
  InstallButton->setText(QString(installSDKAndTargetTip));
  InstallButton->setChecked(true);

   main_devCheckBox=new QCheckBox;
   main_devCheckBox->setText(QString("main_dev"));
   main_devCheckBox->setChecked(true);
   xuanwuCheckBox=new QCheckBox;
   xuanwuCheckBox->setText(QString("xuanwu"));
   miit_certCheckBox=new QCheckBox;
   miit_certCheckBox->setText(QString("miit_cert"));
   product_lilyCheckBox=new QCheckBox;
   product_lilyCheckBox->setText(QString("product_lily"));
   ginkgo_milletCheckBox=new QCheckBox;
   ginkgo_milletCheckBox->setText(QString("ginkgo_millet"));

   archGroupBox=new QGroupBox(QString("平台架构"));
   archGroupBox->setCheckable(true);


   armv7CheckBox=new QCheckBox;
   armv7CheckBox->setText(QString("armv7tnhl"));

   i686CheckBox=new QCheckBox;
   i686CheckBox->setText(QString("i686"));

   targetBranchNameBox=new QGroupBox(QString("TARGET分支名"));
   InstallChooseBox=new QGroupBox(QString("安装选项"));

   buttonBox=new QDialogButtonBox();
   installButton=new QPushButton(QString("安装"));

   cancelButton=new QPushButton(QString("取消"));
   buttonBox->addButton(installButton,QDialogButtonBox::YesRole);
   buttonBox->addButton(cancelButton,QDialogButtonBox::NoRole);

   timer_check=new QTimer;
   timer_check->start(2000);
}

QVBoxLayout* OnlineInstallWidget::createLayout()
{

     QFormLayout*formLayoutSdkPath=new QFormLayout;
     formLayoutSdkPath->addRow(QString(sdkInstallPathTip),sdkPathLineEdit);
     formLayoutSdkPath->setSpacing(10);

    QVBoxLayout* vLayoutInstallChoose=new QVBoxLayout;
    vLayoutInstallChoose->addWidget(InstallButton);
    vLayoutInstallChoose->addWidget(onlyInstallSDKButton);
    vLayoutInstallChoose->addWidget(onlyInstallTargetButton);
    vLayoutInstallChoose->addStretch();
    vLayoutInstallChoose->setSpacing(15);
    InstallChooseBox->setLayout(vLayoutInstallChoose);

    QVBoxLayout* vLayoutArchChoose=new QVBoxLayout;
    vLayoutArchChoose->addWidget(armv7CheckBox);
    vLayoutArchChoose->addWidget(i686CheckBox);
    vLayoutArchChoose->addStretch();
    vLayoutArchChoose->setSpacing(15);
    archGroupBox->setLayout(vLayoutArchChoose);

    QVBoxLayout*vLayoutBranchNameChoose=new QVBoxLayout;
    vLayoutBranchNameChoose->addWidget(main_devCheckBox);
    vLayoutBranchNameChoose->addWidget(xuanwuCheckBox);
    vLayoutBranchNameChoose->addWidget(miit_certCheckBox);
    vLayoutBranchNameChoose->addWidget(product_lilyCheckBox);
    vLayoutBranchNameChoose->addWidget(ginkgo_milletCheckBox);
    vLayoutBranchNameChoose->setSpacing(25);
    vLayoutBranchNameChoose->addStretch();
    targetBranchNameBox->setLayout(vLayoutBranchNameChoose);

    QGridLayout*gLayoutContent=new QGridLayout;
    gLayoutContent->addWidget(InstallChooseBox,0,0,4,3);
    gLayoutContent->addWidget(archGroupBox,3,0,3,3);
    gLayoutContent->addWidget(targetBranchNameBox,0,4,6,3);
    gLayoutContent->setHorizontalSpacing(50);
    gLayoutContent->setVerticalSpacing(50);


    QHBoxLayout *hBoxLayoutButton=new QHBoxLayout;
    hBoxLayoutButton->addWidget(buttonBox);
    hBoxLayoutButton->addSpacing(30);

    QVBoxLayout *vLayoutWhole=new QVBoxLayout;
    vLayoutWhole->addLayout(formLayoutSdkPath);
    vLayoutWhole->addLayout(gLayoutContent);
    vLayoutWhole->addSpacing(30);
    vLayoutWhole->addLayout(hBoxLayoutButton);
    vLayoutWhole->setContentsMargins(80,40,0,20);

    return vLayoutWhole;

}
void OnlineInstallWidget::initUI()
{
   createComponent();
    QVBoxLayout* gLayout=createLayout();
    this->setLayout(gLayout);
}

void OnlineInstallWidget::initConnect()
{
  connect(cancelButton,SIGNAL(clicked()),this,SIGNAL(quitWindow()));
  connect(timer_check,SIGNAL(timeout()),this,SLOT(checkButtonEnable()));

}

void OnlineInstallWidget::checkButtonEnable()
{
        qDebug()<<"sssssssss";
         if(!armv7CheckBox->isChecked())
         {
             qDebug()<<"ischeck********\n";
                    installButton->setEnabled(false);
         }else
         {
             qDebug()<<"not check********\n";
                    installButton->setEnabled(true);
         }
}




















