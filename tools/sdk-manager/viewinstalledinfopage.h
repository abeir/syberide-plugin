#ifndef VIEWINSTALLEDINFOPAGE_H
#define VIEWINSTALLEDINFOPAGE_H
#include "installbasepage.h"
#include<QWidget>
#include<QTableWidget>
#include<QLabel>


class viewInstalledInfoPage : public InstallBasePage
{
  Q_OBJECT

public:
    viewInstalledInfoPage(QWidget *parent = 0);
    void initUI();
//    void initConnect();
    void initializePage();
    bool parseConfigFile(QString configFileName);
    void addrowForTableWidget(const QString &name,const QString &arch,const QString & branchName);

private:
    QTableWidget *installedInfoTableWidget;
    QLabel *m_msgLabel;
    QHeaderView *headerView;

};

#endif // VIEWINSTALLEDINFOPAGE_H
