#include "installbasepage.h"
#include<QStyle>
#include<QAbstractButton>

QMultiMap<QString,QString> InstallBasePage::selectionData;
QString InstallBasePage::sdkPathTip;

InstallBasePage::InstallBasePage(QWidget *parent) :
    QWizardPage(parent)
{
    QColor defaultColor=style()->standardPalette().text().color();
    m_titleColor=defaultColor.name();
    m_complete=true;

    setPixmap(QWizard::WatermarkPixmap,QPixmap(QString(QLatin1String(":/icon/watermark"))));
    setPixmap(QWizard::LogoPixmap,QPixmap(QString(QLatin1String(":/icon/logo"))));

    this->setFixedHeight(240);
}

void InstallBasePage::setColoredTitle(const QString &title)
{
    setTitle(QString::fromLatin1("<font color=\"%1\">%2</font>").arg(m_titleColor, title));
}


void InstallBasePage::setColoredSubTitle(const QString &subTitle)
{
    setSubTitle(QString::fromLatin1("<font color=\"%1\">%2</font>").arg(m_titleColor, subTitle));
}

bool InstallBasePage::isComplete() const
{
        return m_complete;
}

void InstallBasePage::setComplete(bool complete)
{
        m_complete=complete;

        if (QWizard *w = wizard()) {
            if (QAbstractButton *cancel = w->button(QWizard::CancelButton)) {
                if (cancel->hasFocus()) {
                    if (QAbstractButton *next = w->button(QWizard::NextButton))
                        next->setFocus();
                }
            }
        }
        emit completeChanged();
}


QString InstallBasePage::productName() const
{
    return tr("交叉编译套件安装管理");//return tr("PDK-Install-Manager");
}
