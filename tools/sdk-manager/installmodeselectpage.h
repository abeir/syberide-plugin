#ifndef INSTALLMODESELECTPAGE_H
#define INSTALLMODESELECTPAGE_H

#include "installbasepage.h"
#include <QWizardPage>
#include<QLabel>
#include<QRadioButton>

class InstallModeSelectPage : public InstallBasePage
{
    Q_OBJECT

public:
    explicit InstallModeSelectPage(QWidget *parent = 0);
    void initUI();
    void initializePage();
    int nextId() const;

signals:

public slots:

private:
    QLabel *m_msgLabel;
  //  QLabel *explainTipLabel;
    QRadioButton *onlineInstallButton;
    QRadioButton *localInstallButton;
    QRadioButton* viewInstalledInfoButton;

};

#endif // INSTALLMODESELECTPAGE_H
