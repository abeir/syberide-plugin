#include "licensewidget.h"
#include "ui_licensewidget.h"
#include <QFile>
#include <QCoreApplication>

LicenseWidget::LicenseWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseWidget)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    this->setWindowTitle(tr("协议许可"));

    QString strExePath = QCoreApplication::applicationDirPath();
    int nPos = strExePath.lastIndexOf(QLatin1String("/"));
    strExePath = strExePath.mid(0, nPos) ;
    nPos = strExePath.lastIndexOf(QLatin1String("/"));
    strExePath = strExePath.mid(0, nPos) ;

    QString strLicensesPath = strExePath + QLatin1String("/Licenses/license.txt");
    QFile file(strLicensesPath);
    if(file.open(QIODevice::ReadOnly))
    {
        QString strText = QString::fromLocal8Bit(file.readAll());
        ui->textEdit->setText(strText);
        file.close();
    }
}

LicenseWidget::~LicenseWidget()
{
    delete ui;
}
