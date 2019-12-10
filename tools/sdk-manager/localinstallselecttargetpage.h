#ifndef LOCALINSTALLSELECTTARGETPAGE_H
#define LOCALINSTALLSELECTTARGETPAGE_H
#include "installbasepage.h"
#include<QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>

class localinstallselecttargetpage : public InstallBasePage
{
    Q_OBJECT

public:
    localinstallselecttargetpage(QWidget *parent = 0);
    void initUI();
    void initConnect();
    QString selectpackages(QString tiptext);
    bool validatePage();
    void initializePage();

public Q_SLOTS:
    void selectSDKInstallPackages();
    void selectTargetInstallPackages();

private:
    QLineEdit *targetpackgePathLineEdit;
    QLineEdit *sdkpackagePathLineEdit;
    QLabel *sdkpackageTip;
    QLabel *targetpackageTip;
    QPushButton *sdkpackageBrowseButton;
    QPushButton *targetpackageBrowseButton;
    QString sdkfileName;
     QList<QString> targetfileNames;
};

#endif // LOCALINSTALLSELECTTARGETPAGE_H
