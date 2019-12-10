#ifndef INSTALLOPTIONSPAGE_H
#define INSTALLOPTIONSPAGE_H

#include "installbasepage.h"
#include<QLabel>
#include<QRadioButton>


class InstallOptionsPage : public InstallBasePage
{
    Q_OBJECT

public:
    explicit InstallOptionsPage(QWidget *parent=0);
    void initUI();
    void initConnect();
    void initializePage();
    ~InstallOptionsPage();

private Q_SLOTS:

    void setBothTip();
    void setOnlyTargetTip();

private:
   QLabel *msgLabel;
   QRadioButton *onlyInstallTargetButton;
   QRadioButton *InstallSDKAndTargetButton;
};

#endif // INSTALLOPTIONSPAGE_H
