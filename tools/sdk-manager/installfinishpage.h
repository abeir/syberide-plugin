#ifndef INSTALLFINISHPAGE_H
#define INSTALLFINISHPAGE_H

#include<QWidget>
#include<QLabel>
#include"installbasepage.h"

class InstallFinishPage : public InstallBasePage
{
    Q_OBJECT

public:
    InstallFinishPage(QWidget *parent = 0);
    void initUI();
    void entering();

private:
     QLabel *m_msgLabel;
};

#endif // INSTALLFINISHPAGE_H
