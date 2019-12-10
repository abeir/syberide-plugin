#include "performinstallpage.h"
#include "constField.h"

#include<QVBoxLayout>
#include<QDebug>
#include<QProcess>
#include<QMessageBox>
#include<QApplication>
#include<QAbstractButton>
#include<QDir>

PerformInstallPage::PerformInstallPage(QWidget *parent):
 InstallBasePage(parent)
{
    this->initUI();
    this->initConnect();
}

void PerformInstallPage::initUI()
{
    setColoredTitle(tr(" %1").arg(productName()));
    setColoredSubTitle(tr("installing，please wait....,if install finished,the page is change."));
    setPixmap(QWizard::WatermarkPixmap,QPixmap());

    process=NULL;


    m_waitLabel=new QLabel(this);
    movie =new QMovie(tr(":/icon/waiting"));
    m_waitLabel->setMovie(movie);
    m_msgLabel = new QLabel(this);
    m_msgLabel->setWordWrap(true);
    QFont font;
    font.setBold(true);
    m_msgLabel->setFont(font);
    m_msgLabel->setText(tr("Installing......"));

    QHBoxLayout *hLayout=new QHBoxLayout;
     hLayout->addWidget(m_waitLabel);
     hLayout->addWidget(m_msgLabel);
     hLayout->addItem(new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    installInfoBrowser=new QTextBrowser;
    installInfoBrowser->setFixedHeight(160);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout);
    vLayout->addSpacing(20);
    vLayout->addWidget(installInfoBrowser);
    vLayout->addItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->setLayout(vLayout);
    setCommitPage(true);

}

void PerformInstallPage::initializePage()
{
    setComplete(false);
    setCommitPage(false);

//    QMultiMap<QString,QString>::const_iterator iterator=selectionData.cbegin();
//    while(iterator!=selectionData.cend())
//    {
//            qDebug()<<"key::"<<iterator.key()<<"value::"<<iterator.value();
//            iterator++;
//    }
//    qDebug()<<"keys count::"<<selectionData.keys().count();

    movie->start();
    setColoredTitle(tr("Installing %1").arg(productName()));
    QString password=field(PasswordLineEditText).toString();
    QString sdkinstallPath=field(SDKPathLineEditText).toString();
    bool mode=field(localInstallMode).toBool();
    if(mode)
    {
        this->parselocalArgumentList();

    }else
    {
         this->parseOnlineArgumentsList();
        if(field(onlyTarget).toBool())
        {
                this->arguments<<QString(QLatin1String("--only-install-target"));
        }
    }
     this->startProcessInstall(password,sdkinstallPath);

}

void PerformInstallPage::parselocalArgumentList()
{
    this->arguments.clear();
     bool modeoption=field(bothSDKTarget).toBool();
     if(modeoption)
     {
            QString sdkPackageName=field(sdkpackagePath).toString();
            this->arguments<<QString(QLatin1String("--sdk-package"));
            this->arguments<<sdkPackageName;
     }
     QString targetPackageNames=field(targetpackgePath).toString();
     QStringList targetPackages=targetPackageNames.split(QString(QLatin1String(";")));
     foreach (QString targetPackage, targetPackages) {
            this->arguments<<QString(QLatin1String("--target-package"));
            this->arguments<<targetPackage;
     }
}

void PerformInstallPage::parseOnlineArgumentsList()
{
//    QStringList arguments;
    this->arguments.clear();
    QMultiMap<QString,QString>::const_iterator iterator=selectionData.cbegin();
   while(iterator!=selectionData.cend())
   {
       QVariant data=iterator.key();
       data.toString();
       arguments<<QString(QLatin1String("--archbranch"))<<data.toString();
       data=iterator.value();
       arguments<<data.toString();
        iterator++;
   }

}

void PerformInstallPage::startProcessInstall(QString password,QString sdkinstallPath)
{
//     QStringList arguments;
//    QMultiMap<QString,QString>::const_iterator iterator=selectionData.cbegin();
//    while(iterator!=selectionData.cend())
//    {
//        QVariant data=iterator.key();
//        data.toString();
//        arguments<<QString("--archbranch")<<data.toString();
//        data=iterator.value();
//        arguments<<data.toString();
//         iterator++;
//    }
//    arguments=parseOnlineArgumentsList();
    process=new QProcess;
    connect(process,SIGNAL(started()),this,SLOT(startProgram()));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStandardOutputData()));

    arguments<<QString(QLatin1String("--silent"))<<QString(QLatin1String("--sudo-password"))<<password<<QString(QLatin1String("--sdk-install-path"))<<sdkinstallPath;

    QString appPath=QApplication::applicationDirPath();
    QDir appdir(appPath);
    QString program=appdir.absolutePath()+QLatin1String("/PDKInstallManager");

    process->start(program,arguments);
    process->waitForStarted();
}

void PerformInstallPage::readStandardOutputData()
{
         static QByteArray buffer(1024,'\0');
         while(process->bytesAvailable())
         {
              process->readLine(buffer.data(),buffer.size());
             installInfoBrowser->append(QString(QLatin1String(buffer.data())));
         }

}

bool PerformInstallPage::validatePage()
{
    this->wizard()->button(QWizard::CommitButton)->setEnabled(false);
    return true;
}

void PerformInstallPage::processError(QProcess::ProcessError)
{
      disconnect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));

     int ret=QMessageBox::warning(this,tr("error"),tr("The start install is failed!"));
     this->killProcess();

     if(ret>0)
        qApp->quit();

}

void PerformInstallPage::startProgram()
{
    disconnect(process,SIGNAL(started()),this,SLOT(startProgram()));
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishProgram(int,QProcess::ExitStatus)));
}

void PerformInstallPage::finishProgram(int exitCode, QProcess::ExitStatus exitStatus)
{

    movie->stop();
    disconnect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishProgram(int,QProcess::ExitStatus)));
    disconnect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    if((exitCode==0)&&(exitStatus==QProcess::NormalExit))
    {
                delete process;
                process=NULL;

                emit this->installFinished();
    }else
    {
        switch(exitCode){
        case 2:
            QMessageBox::warning(this,tr("error"),tr("The exec install is failed! Maybe not install expect. "));
         break;
       default:
              QMessageBox::warning(this,tr("error"),tr("The exec install is failed! "));
         break;
        }
         this->killProcess();
         qApp->quit();
   }
}

void PerformInstallPage::killProcess()
{
    if(process)
     {
        if((process->state()==QProcess::Running)||(process->state()==QProcess::Starting))
             process->kill();
    }

}

PerformInstallPage::~PerformInstallPage()
{
        this->killProcess();
}

void PerformInstallPage::initConnect()
{

}
