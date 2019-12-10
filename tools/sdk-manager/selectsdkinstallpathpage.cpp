#include "selectsdkinstallpathpage.h"
#include "pdkinstallwizard.h"
#include "constField.h"

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QDir>
#include<QFileDialog>
#include<QFormLayout>
#include<QMessageBox>
#include<QSettings>
#include<QDebug>
#include <security/pam_appl.h>


static const   QString chrootFile(QLatin1String("/sdk/sdk-root/syberos-sdk-chroot"));

struct ad_user {
    char *login;
    char *passwd;
};

SelectSDKInstallPathPage::SelectSDKInstallPathPage(QWidget *parent):
 InstallBasePage(parent)
{
        this->initUI();
        this->initConnect();
}

void SelectSDKInstallPathPage::initUI()
{
        setPixmap(QWizard::WatermarkPixmap,QPixmap());
        setColoredTitle(tr(" %1").arg(productName()));
        //setColoredSubTitle(tr(" sdk is the basic tool chain package.target is platform compiler environment."));

        browseButton=new QPushButton(tr("Browse"));
        sdkPathLineEdit=new QLineEdit;
        sdkPathLineEdit->setReadOnly(true);
        sdkPathLineEdit->setFixedWidth(350);
        sdkPathTipLabel=new QLabel(tr("请选择安装路径:"));//sdkPathTipLabel=new QLabel(tr("please select sdk install path:"));
        verifyPasswdTip=new QLabel(tr("安装过程需要 root 权限"));//verifyPasswdTip=new QLabel(tr("This step will be verify password"));

       QString sdkInstallPath=QDir::homePath()+QString(QLatin1String("/SyberOS-Pdk"));
        sdkPathLineEdit->setText(sdkInstallPath);

        QHBoxLayout *hLayout=new QHBoxLayout;
        hLayout->addStretch();
        hLayout->addWidget(sdkPathLineEdit);
        hLayout->addWidget(browseButton);
        hLayout->addItem(new QSpacerItem(60, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        hLayout->setContentsMargins(0,0,0,0);

        QFormLayout* formLayout=new QFormLayout;
        passwordLineEdit=new QLineEdit;
        passwordLineEdit->setEchoMode(QLineEdit::Password);
        passwordLineEdit->setFixedWidth(250);
        formLayout->addRow(tr("当前用户密码:"),passwordLineEdit);//formLayout->addRow(tr("Current user's' Password:"),passwordLineEdit);

        QVBoxLayout *vLayout=new QVBoxLayout;
        vLayout->addWidget(sdkPathTipLabel);
        vLayout->addLayout(hLayout);
        vLayout->addSpacing(30);
        vLayout->addLayout(formLayout);
        vLayout->addWidget(verifyPasswdTip);
        vLayout->addItem(new QSpacerItem(10, 150, QSizePolicy::Minimum, QSizePolicy::Expanding));
        this->setLayout(vLayout);

        registerField(SDKPathLineEditText,sdkPathLineEdit);
        registerField(PasswordLineEditText,passwordLineEdit);
}

void SelectSDKInstallPathPage::selectSDKInstallPath()
{

   QString  sdkPath=QFileDialog::getExistingDirectory(this,tr("select sdk install path Dialog"),QDir::homePath(), QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    if(!sdkPath.isEmpty())
    {
        sdkPathLineEdit->setText(sdkPath);
    }
}

void SelectSDKInstallPathPage::initializePage()
{
        sdkPathTipLabel->setText(sdkPathTip);
        verifyPasswdTip->setText(tr("This step will be verify password"));//verifyPasswdTip->setText(tr("安装过程需要 root 权限"));
        bool modeoption=field(onlyTarget).toBool();
        if(modeoption)
        {
                QSettings setting(QSettings::NativeFormat,QSettings::UserScope,QString(QLatin1String("qtversions")));
                QFile configFile(setting.fileName());
                QString installPath;
                if(configFile.exists())
                {
                    if(!configFile.open(QIODevice::ReadOnly|QIODevice::Text))
                    {
                        QMessageBox::warning(0, QMessageBox::tr("the file is open error!"), QMessageBox::tr("the file is open error! "));
                    }
                    while(!configFile.atEnd())
                    {
                            QByteArray line=configFile.readLine();
                            if(line.contains("sdk-install-path"))
                            {
                                installPath=QString::fromLatin1(line);
                                break;
                            }
                    }
                }
                configFile.close();
                if(!installPath.isEmpty())
                {
                    int pos=installPath.indexOf(QString(QLatin1String("=")));
                    QString sdkPath=installPath.mid(pos+1).trimmed();
                    sdkPathLineEdit->setText(sdkPath);
                }
        }
}

int PAM_conv (int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
    struct ad_user *user= (struct ad_user *)appdata_ptr;
    struct pam_response *response;
    int i;

    /* Sanity checking */
    if (msg == NULL || resp == NULL || user == NULL)
        return PAM_CONV_ERR;

    response= (struct pam_response *)
        malloc(num_msg * sizeof(struct pam_response));

    for (i= 0; i < num_msg; i++)
    {
        response[i].resp_retcode= 0;
        response[i].resp= NULL;

        switch ((*(msg[i])).msg_style)
        {
        case PAM_PROMPT_ECHO_ON:
            /* Store the login as the response */
            /* This likely never gets called, since login was on pam_start() */
            response[i].resp= appdata_ptr ? (char *)strdup(user->login) : NULL;
            break;

        case PAM_PROMPT_ECHO_OFF:
            /* Store the password as the response */
            response[i].resp= appdata_ptr ? (char *)strdup(user->passwd) : NULL;
            break;

        case PAM_TEXT_INFO:
        case PAM_ERROR_MSG:
            /* Shouldn't happen since we have PAM_SILENT set. If it happens
             * anyway, ignore it. */
            break;

        default:
            /* Something strange... */
            if (response != NULL) free(response);
            return PAM_CONV_ERR;
        }
    }

    /* On success, return the response structure */
    *resp= response;
    return PAM_SUCCESS;
}

bool SelectSDKInstallPathPage::checkUserPassword(const QString &password)
{
    char *login = getenv("USER");
    if (login == NULL)
        return false;

    QByteArray ba = password.toLatin1();
    struct ad_user user_info = { login, ba.data() };
    struct pam_conv conv = { PAM_conv, (void *)&user_info };
    pam_handle_t *pamh = NULL;

    int retval = pam_start("pwauth", login, &conv, &pamh);

    if (retval == PAM_SUCCESS)
        retval= pam_authenticate(pamh, PAM_SILENT);

    if (retval == PAM_SUCCESS)
        retval= pam_acct_mgmt(pamh, 0);     /* permitted access? */

    if (pam_end(pamh,retval) != PAM_SUCCESS)    /* close PAM */
    {
        pamh= NULL;
        return false;
    }

    /* On failure we always return STATUS_UNKNOWN although we can't tell
     * if the failure was because of a bad login or a bad password */
    return retval == PAM_SUCCESS;
}

int SelectSDKInstallPathPage::nextId() const
{
    bool localInstallFlag=field(localInstallMode).toBool();
    if(localInstallFlag)
        return PDKInstallWizard::LocalInstallSelectTargetPageID;

    return PDKInstallWizard::SelectTargetInstallPageID;
}

bool SelectSDKInstallPathPage::validatePage()
{

         if(!checkUserPassword(passwordLineEdit->text()))
         {
                verifyPasswdTip->setText(tr("the input  Password is wrong!!!"));
                verifyPasswdTip->show();
                QMessageBox::warning(0, QMessageBox::tr("The password is wrong!"), QMessageBox::tr("The input password is wrong!"));
                return false;
         }
         bool mode=field(QString(QLatin1String("onlyTarget"))).toBool();
         if(mode)
         {
                QString sdkInstallPath=sdkPathLineEdit->text();

                if(!QFile(sdkInstallPath+chrootFile).exists())
                {
                    QMessageBox::warning(0, QMessageBox::tr("The select is not correct!"), QMessageBox::tr("The  %1%2 is not exist! ").arg(sdkInstallPath).arg(chrootFile));
                    return false;
                }
         }
         return true;
}

void SelectSDKInstallPathPage::initConnect()
{
        connect(browseButton,SIGNAL(clicked()),this,SLOT(selectSDKInstallPath()));
}
