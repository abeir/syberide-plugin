#ifndef PERFORMINSTALLPAGE_H
#define PERFORMINSTALLPAGE_H
#include "installbasepage.h"
#include "qpdktargetitemmodel.h"
#include<QWidget>
#include<QLabel>
#include<QProcess>
#include<QMultiMap>
#include<QTextBrowser>
#include<QMovie>

class PerformInstallPage : public InstallBasePage
{
    Q_OBJECT

public:
    PerformInstallPage(QWidget *parent = 0);
    void initUI();
    void initConnect();
    void startProcessInstall(QString password,QString sdkinstallPath);
    void initializePage();
    bool validatePage();
    void parseOnlineArgumentsList();
    void parselocalArgumentList();
 //  bool isComplete();
   ~PerformInstallPage();

protected:
   //void closeEvent(QCloseEvent *event);

private slots:
    void startProgram();
    void finishProgram(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError);
    void readStandardOutputData();

signals:
    void installFinished();

private:
    void killProcess();



private:
    QLabel *m_msgLabel;
    QLabel *m_waitLabel;
    QMovie *movie;
    QProcess* process;
    QMultiMap<QString,QString> targetBranchInfo;
    QPDKTargetItemModel *PDKTargetModel;
    QTextBrowser *installInfoBrowser;
     QStringList arguments;

};

#endif // PerformInstallPage_H
