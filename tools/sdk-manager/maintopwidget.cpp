#include "maintopwidget.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

MainTopWidget::MainTopWidget(QWidget *parent) :
    QWidget(parent)
{
    this->initUI();
    this->initConnect();
}

void MainTopWidget::initUI()
{
    this->setObjectName("basewidget");
//    m_logoLabel = new QLabel;
//    //m_logoLabel->setMinimumSize(161, 32);
//    m_logoLabel->setFixedSize(188, 34);
//    m_logoLabel->setObjectName("logolabel");

//    m_menuButton = new QPushButton;
//    m_menuButton->setObjectName("menuButton");
//    m_menuButton->setFixedSize(27, 22);
    m_minButton = new QPushButton;
    m_minButton->setFixedSize(27, 22);
    m_minButton->setObjectName("minButton");
    m_closeButton = new QPushButton;
    m_closeButton->setFixedSize(27, 22);
    m_closeButton->setObjectName("closeButton");


//    QVBoxLayout *vLogo = new QVBoxLayout;
   // vLogo->addWidget(m_logoLabel);
 //   vLogo->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *hlayout = new QHBoxLayout;

 //   hlayout->addWidget(m_menuButton, 0, Qt::AlignTop);
    hlayout->addWidget(m_closeButton, 0, Qt::AlignTop);
    hlayout->addWidget(m_minButton, 0, Qt::AlignTop);
    hlayout->addStretch();
    hlayout->setContentsMargins(5, 5, 5, 5);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addStretch();
    vlayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(vlayout);
    this->setFixedHeight(40);
}

void MainTopWidget::initConnect()
{
  //  connect(m_menuButton, SIGNAL(clicked()), this, SLOT(showMenuSlot()));
    connect(m_minButton, SIGNAL(clicked()), this, SIGNAL(minMain()));
    connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(closeMain()));
}

void MainTopWidget::setMenuDown(bool isDown)
{
    m_menuButton->setDown(isDown);
}

void MainTopWidget::showMenuSlot()
{
}

void MainTopWidget::paintEvent(QPaintEvent *)
{
    //解决不能使用样式表设置背景图片 参考http://blog.csdn.net/dbzhang800/article/details/6875352
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


