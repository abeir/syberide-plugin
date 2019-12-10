#ifndef SELECTSDKINSTALLPATHPAGE_H
#define SELECTSDKINSTALLPATHPAGE_H
#include "installbasepage.h"
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QUrl>

class SelectSDKInstallPathPage : public InstallBasePage
{
    Q_OBJECT

public:
    SelectSDKInstallPathPage(QWidget *parent = 0);
    void initUI();
    void initConnect();
    int nextId() const;
    bool checkUserPassword(const QString &password);
    bool validatePage();
    void initializePage();

public slots:
    void selectSDKInstallPath();
  //  void message();

private:
    QLineEdit * sdkPathLineEdit;
    QLineEdit *passwordLineEdit;
    QLabel *sdkPathTipLabel;
    QLabel *verifyPasswdTip;
    QPushButton *browseButton;
 //   QUrl sdkInstallPath;

};

#endif // SELECTSDKINSTALLPATH_H
