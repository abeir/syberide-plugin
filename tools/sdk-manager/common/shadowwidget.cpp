#include "shadowwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <qmath.h>

ShadowWidget::ShadowWidget(QWidget *parent) :
    QDialog(parent), m_mousePress(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
}

void ShadowWidget::reject()
{
    emit closeWidget();
}

void ShadowWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ShadowWidget::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        m_mousePress = true;
    }
    //窗口移动距离
    m_movePoint = event->globalPos() - pos();
}

void ShadowWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePress = false;
}

void ShadowWidget::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(m_mousePress)
    {
        QPoint movePpos = event->globalPos();
        move(movePpos - m_movePoint);
    }
}

