#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H
#include "shadowwidget.h"
#include<QTextEdit>
class QLabel;
class QPushButton;

namespace SyberSDKUpdate {
namespace Internal {
class UpdateDialog : public ShadowWidget
{
    Q_OBJECT
public:
    explicit UpdateDialog(QWidget *parent = 0);
    void setNoupdateTiptext();
    void setUpdateTipText();
    void setNetworkErrorInfoText();
    void parseChangelog(QString fileName);
    void closeDialog(bool flag);
private slots:
    void execUpdate(bool clickflag);
    void cancelUpdate(bool clickflag);
    void closeWidget(bool clickflag);
    void writeNotipFlag(bool clickflag);
signals:
    void  execupdate();
    void writeNoTipFlag();

private:
    void initUI();
    void initConnect();
    void buttonShowOrHide(bool flag);
private:
    QLabel *m_titleiconLabel;
    QLabel *m_titleLabel;
    QTextEdit * m_messageTextedit;

    QPushButton *m_closeButton;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_tipButton;
};
}
}
#endif // UPDATEDIALOG_H
