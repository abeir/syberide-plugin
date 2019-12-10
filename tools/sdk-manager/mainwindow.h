#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include<QTabWidget>
#include "common/shadowwidget.h"
#include "maintopwidget.h"
#include "onlineInstallWidget.h"


class MainWindow : public ShadowWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void showWidget();
public slots:
    /*!
     * \brief 当按esc键会调用reject
     */
    void reject();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void turnPage(int page);
    /*!
     * \brief close main window
     */
    void closeWindow();
    void showMainMenu(QPoint point);
    void showMiniWidget();
    void runingStyle(int style);
    void runingEnd();
    void showAboutUs();
signals:
    void sendRuningStyle(int);//用来发送任务处理
    void sendRuningEnd();
private:
    void initUI();
    void initConnect();
private:
    MainTopWidget *m_mainTopWidget;
    QTabWidget * MainTabWidget;
    OnlineInstallWidget *onlineInstallWidget;

    int m_runingStyle; //正在进行的操作
    bool m_isRuning;
};

#endif // MAINWINDOW_H
