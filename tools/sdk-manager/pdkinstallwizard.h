#ifndef PDKINSTALLWIZARD_H
#define PDKINSTALLWIZARD_H

#include <QWizard>
#include "selecttargetinstallpage.h"
#include "performinstallpage.h"

class PDKInstallWizard : public QWizard
{
    Q_OBJECT

public:
    explicit PDKInstallWizard(QWidget *parent = 0);

    enum WizardPage {
        InstallModePageID,
        ViewInstalledInfoPageID,
        InstallOptionsPageID ,
        SelectSDKPathPageID ,
        SelectTargetInstallPageID ,
        LocalInstallSelectTargetPageID,
        PerformInstallPageID ,
        InstallationFinishedPageID ,
        End
    };


signals:

public Q_SLOTS:
    void performCorrespondPage(int id);
    void showFinishedPage();

private:
    PerformInstallPage * performInstallPage;
    int currentPageID;

};

#endif // SDKINSTALLWIZARD_H
