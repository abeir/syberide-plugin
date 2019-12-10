#include "maintoolgroupwidget.h"
#include <QPushButton>
#include <QSignalMapper>
#include <QHBoxLayout>
#include <QLabel>

MainToolGroupWidget::MainToolGroupWidget(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    QStringList objList;
    objList << ":/buttons/homepage" << ":/buttons/hardware" << ":/buttons/cleanpc"\
            << ":/buttons/optimization" << ":/buttons/systemmonitor";
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i=0; i != objList.size(); i++)
    {
//        HToolButton *button = new HToolButton(objList.at(i));
//        m_toolList.append(button);
//        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
//        signalMapper->setMapping(button, i);
//        hlayout->addWidget(button);
        HButton *button = new HButton(objList.at(i));
        m_buttonList.append(button);
        button->setMinimumHeight(50);
        connect(button, SIGNAL(buttonClicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i);
        hlayout->addWidget(button);

        if(i != (objList.size() - 1))
        {
            //添加分割线
            QLabel *label = new QLabel;
            label->setObjectName("line");
            label->setFixedSize(3, 48);
            hlayout->addWidget(label);
        }
    }
    hlayout->setContentsMargins(0, 5, 0, 0);
    hlayout->setSpacing(0);//设置控件间距离为0
    connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(turnPageSignal(int)));
    this->setLayout(hlayout);

    m_buttonList.at(0)->setMousePress(true);
}

void MainToolGroupWidget::turnPage(int page)
{
    for(int i=0; i != m_buttonList.size(); i++)
    {
        if(page == i)
        {
            m_buttonList.at(i)->setMousePress(true);
        }else
        {
            m_buttonList.at(i)->setMousePress(false);
        }
    }
}
