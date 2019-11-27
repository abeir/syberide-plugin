#ifndef BASESTYLEWIDGET_H
#define BASESTYLEWIDGET_H

#include <QWidget>

class BaseStyleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BaseStyleWidget(QWidget *parent = 0);

signals:

protected:
    virtual void paintEvent(QPaintEvent *);

public slots:
};

#endif // BASESTYLEWIDGET_H
