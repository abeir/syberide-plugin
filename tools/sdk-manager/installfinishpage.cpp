#include "installfinishpage.h"
#include<QVBoxLayout>
#include<QWizard>
#include<QAbstractButton>
#include<QDebug>
#include<QMessageBox>

InstallFinishPage::InstallFinishPage(QWidget *parent):
InstallBasePage(parent)
{
    this->initUI();
}

void InstallFinishPage::initUI()
{
    setColoredTitle(tr(" %1").arg(productName()));
    setColoredSubTitle(QString(QLatin1String("")));
    setPixmap(QWizard::WatermarkPixmap,QPixmap());

    m_msgLabel = new QLabel(this);
    m_msgLabel->setWordWrap(true);
    QFont font;
    font.setBold(true);
    m_msgLabel->setFont(font);
    m_msgLabel->setText(tr("Installation complete!\n if installed IDE, IDE need to restart to take effect!"));
   QVBoxLayout *vLayout = new QVBoxLayout(this);
   vLayout->addWidget(m_msgLabel);
   vLayout->addItem(new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));
   this->setLayout(vLayout);

}

void InstallFinishPage::entering()
{
        QMessageBox::information(this,tr("Installation complete"),tr("If installed Syberos IDE, Syberos IDE need to restart to take effect!"),QMessageBox::Ok);
}



