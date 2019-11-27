#include "updatedialog.h"
#include "basestylewidget.h"
#include "constants.h"
#include<QLabel>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QFile>
#include<QDebug>
#include<QTextEdit>
#include<QTextStream>
#include<QApplication>

using namespace SyberSDKUpdate::Internal;
using namespace QUpdate;

UpdateDialog::UpdateDialog(QWidget *parent):ShadowWidget(parent)
{
        this->initUI();
        this->initConnect();
}

void UpdateDialog::initUI()
{
    this->setObjectName(tr("basewidget"));
    this->setFixedSize(500,250);
    //this->setWindowTitle(tr("Update Dialog"));

    m_titleLabel= new QLabel;
    m_titleLabel->setText(tr("SyberOS SDK 升级"));
    m_titleLabel->setObjectName(tr("DialogTitle"));

    m_closeButton=new QPushButton;
    m_closeButton->setObjectName(tr("closeButton"));
    m_closeButton->setFixedSize(27,22);
    m_closeButton->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *titleLayout=new QHBoxLayout;
//    titleLayout->addSpacing(15);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_closeButton);
    titleLayout->setContentsMargins(5,5,5,5);

    m_messageTextedit=new QTextEdit;
    m_messageTextedit->setObjectName(tr("messageText"));


    m_cancelButton=new QPushButton;
    m_cancelButton->setObjectName(tr("cancelButton"));
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setFocusPolicy(Qt::NoFocus);

    m_okButton=new QPushButton;
    m_okButton->setObjectName(tr("okButton"));
    m_okButton->setText(tr("Update"));
    m_okButton->setFocusPolicy(Qt::NoFocus);

    m_tipButton=new QPushButton;
    m_tipButton->setObjectName(tr("tipButton"));
    m_tipButton->setText(tr("No tips"));
    m_tipButton->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *buttonLayout=new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_tipButton);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->setContentsMargins(5,5,5,2);

    QVBoxLayout *contentVLayout=new QVBoxLayout;
    contentVLayout->addStretch(20);
    contentVLayout->addWidget(m_messageTextedit);
    contentVLayout->addLayout(buttonLayout);

    BaseStyleWidget *contentWidget=new BaseStyleWidget;
    contentWidget->setObjectName(tr("backgroundWidget"));

    contentWidget->setLayout(contentVLayout);

    QVBoxLayout *vLayout=new QVBoxLayout;
    vLayout->addLayout(titleLayout);
    vLayout->addWidget(contentWidget);
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->setSpacing(0);

    this->setLayout(vLayout);

    //加载QSS样式表
    QFile qss(QLatin1String(":/update.css"));
    qss.open(QFile::ReadOnly);

    QByteArray byte=qss.readAll();
    QString appStyle=QLatin1String(byte);

    this->setStyleSheet(appStyle);
    qss.close();

}

void UpdateDialog::setNetworkErrorInfoText()
{
    if(m_messageTextedit)
    {
        m_messageTextedit->setText(tr("NetworkError or File not exist"));
        m_messageTextedit->setReadOnly(true);
    }
        this->buttonShowOrHide(false);

}

void UpdateDialog::buttonShowOrHide(bool flag)
{
    if(m_tipButton)
        flag ?m_tipButton->show():m_tipButton->hide();
    if(m_cancelButton)
        flag?m_cancelButton->show():m_cancelButton->hide();
   if(m_okButton)
         flag?m_okButton->setText(tr("Update")):m_okButton->setText(tr("Sure"));
}

void UpdateDialog::setNoupdateTiptext()
{
    if(m_messageTextedit)
    {
        m_messageTextedit->setText(tr("No Update"));
        m_messageTextedit->setReadOnly(true);
    }

    this->buttonShowOrHide(false);
}

void UpdateDialog::setUpdateTipText()
{

     this->buttonShowOrHide(true);
//       if(m_tipButton)
//           m_tipButton->show();
//       if(m_cancelButton)
//           m_cancelButton->show();
//       if(m_okButton)
//           m_okButton->setText(tr("Update"));
}

void UpdateDialog::initConnect()
{

    connect(m_cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelUpdate(bool)));
    connect(m_closeButton,SIGNAL(clicked(bool)),this,SLOT(closeWidget(bool)));
    connect(m_okButton,SIGNAL(clicked(bool)),this,SLOT(execUpdate(bool)));
    connect(m_tipButton,SIGNAL(clicked(bool)),this,SLOT(writeNotipFlag(bool)));
}

void UpdateDialog::writeNotipFlag(bool clickflag)
{
    Q_UNUSED(clickflag);
    emit this->writeNoTipFlag();
}

void UpdateDialog::execUpdate(bool clickflag)
{
    Q_UNUSED(clickflag);
   if(m_okButton->text()==tr("Update"))
        emit this->execupdate();
   else
   {
       this->done(0);
       this->close();
   }
}

void UpdateDialog::parseChangelog(QString fileName)
{
        if(fileName==NullString)
        {
            m_messageTextedit->setText(tr("No Changelog"));
            m_messageTextedit->setReadOnly(true);
            return;
        }

        QFile file(fileName);
        if(!file.open(QFile::ReadOnly|QFile::Text))
        {
            qDebug()<<tr("open file error!");
            return;
        }
        QTextStream in(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_messageTextedit->setPlainText(in.readAll());
        QApplication::restoreOverrideCursor();
        m_messageTextedit->setReadOnly(true);
}

void UpdateDialog::closeDialog(bool flag)
{
        this->closeWidget(flag);
}

void UpdateDialog::closeWidget(bool clickflag)
{
     Q_UNUSED(clickflag);

        this->done(0);
        this->close();
}

void UpdateDialog::cancelUpdate(bool clickflag)
{
        Q_UNUSED(clickflag);
          this->done(0);
          this->close();
}
