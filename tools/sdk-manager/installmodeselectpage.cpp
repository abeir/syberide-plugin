#include "installmodeselectpage.h"
#include "constField.h"
#include "pdkinstallwizard.h"
#include<QVBoxLayout>
#include<QSettings>
#include<QFile>

InstallModeSelectPage::InstallModeSelectPage(QWidget *parent) :
    InstallBasePage(parent)
{
    this->initUI();
}

void InstallModeSelectPage::initUI()
{
    setObjectName(QLatin1String("IntroductionPage"));
    //setColoredTitle(tr(" %1").arg(productName()));
    //setColoredSubTitle(tr("PDK is a compilation environment,PDK contain one sdk and multiple target."));
    setColoredTitle(tr("欢迎使用交叉编译套件管理工具").arg(productName()));
    //setColoredSubTitle(tr("SyberOS PDK 是以SyberOS系统为运行环境软件的编译环境，其中包括一个SDK（编译专用的软件开发环境）与一个或多个target（指定版本所需的资源集合）."));

                          //sdk is the basic tool chain package.target is platform compiler environment."));
    QVBoxLayout *layout=new QVBoxLayout(this);
     layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setLayout(layout);

    m_msgLabel = new QLabel(this);
    m_msgLabel->setWordWrap(true);

    QFont font;
    font.setBold(true);
    m_msgLabel->setFont(font);
    //m_msgLabel->setText(tr("Welcome to the %1 Wizard.").arg(productName()));

    
    //m_msgLabel->setText(tr("欢迎使用交叉编译环境管理工具.").arg(productName()));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(widget);

    onlineInstallButton=new QRadioButton(tr("在线安装套件"),this);
    localInstallButton=new QRadioButton(tr("本地安装套件"),this);
    viewInstalledInfoButton=new QRadioButton(tr("查看已安装的套件信息"),this);
    //onlineInstallButton=new QRadioButton(tr("Online Installation"),this);
    //localInstallButton=new QRadioButton(tr("Local Installation"),this);
    //viewInstalledInfoButton=new QRadioButton(tr("view Installed info"),this);
    vLayout->addWidget(onlineInstallButton);
    vLayout->addWidget(localInstallButton);
    vLayout->addWidget(viewInstalledInfoButton);
    onlineInstallButton->setChecked(true);
     vLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

     layout->addWidget(m_msgLabel);
     layout->addWidget(widget);

     layout->addItem(new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding));
     registerField(localInstallMode,localInstallButton);
     registerField(viewInstalledInfo,viewInstalledInfoButton);
}


void InstallModeSelectPage::initializePage()
{
    QSettings setting(QSettings::NativeFormat,QSettings::UserScope,QString(QLatin1String("qtversions")));
    QFile configFile(setting.fileName());
    if(!configFile.exists())
        viewInstalledInfoButton->hide();
    else
        viewInstalledInfoButton->show();
}

int InstallModeSelectPage::nextId() const
{
    if(viewInstalledInfoButton->isChecked())
    {
        return PDKInstallWizard::ViewInstalledInfoPageID;
    }else
    {
        return PDKInstallWizard::InstallOptionsPageID;
    }

}



