#include "mainwindow.h"
#include  "onlineInstallWidget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include<QTabWidget>

MainWindow::MainWindow(QWidget *parent) :
    ShadowWidget(parent), m_isRuning(false)
{
    this->initUI();
    this->initConnect();
}

void MainWindow::initUI()
{
    this->setFixedSize(600, 500);
    this->setWindowTitle(tr("SDK-Manager"));
    ///添加任务栏关闭选项
    this->setWindowFlags(this->windowFlags() | Qt::WindowCloseButtonHint);

    m_mainTopWidget = new MainTopWidget;
    onlineInstallWidget =new OnlineInstallWidget;
    MainTabWidget= new QTabWidget;
    MainTabWidget->addTab(onlineInstallWidget,QString("在线安装"));
    //MainTabWidget->setTabText(0,QString("本地"));
   // MainTabWidget->setTabText(1,QString("在线"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_mainTopWidget);
    mainLayout->addWidget(MainTabWidget);
    //mainLayout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
}

void MainWindow::initConnect()
{
    connect(m_mainTopWidget, SIGNAL(turnPageSignal(int)), this, SLOT(turnPage(int)));
    connect(m_mainTopWidget, SIGNAL(showMenu(QPoint)), this, SLOT(showMainMenu(QPoint)));
    connect(m_mainTopWidget, SIGNAL(minMain()), this, SLOT(showMinimized()));
    connect(m_mainTopWidget, SIGNAL(closeMain()), this, SLOT(closeWindow()));
    connect(onlineInstallWidget,SIGNAL(quitWindow()),this,SLOT(closeWindow()));
}

void MainWindow::runingStyle(int style)
{
    m_runingStyle = style;
    m_isRuning = true;
    emit sendRuningStyle(style);
}

void MainWindow::runingEnd()
{
    m_isRuning = false;
    emit sendRuningEnd();
}

void MainWindow::showWidget()
{
    this->showNormal();
    this->raise();
    this->activateWindow();
}

void MainWindow::turnPage(int page)
{

}

void MainWindow::showAboutUs()
{
    //m_mainTopWidget->setMenuDown(false);
}

void MainWindow::showMainMenu(QPoint point)
{

     //m_mainTopWidget->setMenuDown(false);
}

void MainWindow::closeWindow()
{
        qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->closeWindow();
    event->ignore();
}

void MainWindow::reject()
{
    this->closeWindow();
}

void MainWindow::showMiniWidget()
{

}
