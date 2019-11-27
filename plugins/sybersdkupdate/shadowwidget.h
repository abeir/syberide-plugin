#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H
#include<QDialog>

class ShadowWidget : public QDialog
{
    Q_OBJECT
public:
    explicit ShadowWidget(QWidget * parent=0);
public slots:
    void reject();
signals:
    void closeWidget();
 protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);

 private:
    QPoint m_movePoint;
     bool m_mousePress;
};

#endif // SHADOWWIDGET_H
