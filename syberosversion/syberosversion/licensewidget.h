#ifndef LICENSEWIDGET_H
#define LICENSEWIDGET_H

#include <QDialog>

namespace Ui {
class LicenseWidget;
}

class LicenseWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseWidget(QWidget *parent = 0);
    ~LicenseWidget();

private:
    Ui::LicenseWidget *ui;
};

#endif // LICENSEWIDGET_H
