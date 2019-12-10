#ifndef MAINTOOLGROUPWIDGET_H
#define MAINTOOLGROUPWIDGET_H

#include <QWidget>
#include "htoolbutton.h"
#include "hbutton.h"

class MainToolGroupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainToolGroupWidget(QWidget *parent = 0);
    void turnPage(int page);
signals:
    void turnPageSignal(int);
private:
    QList<HToolButton *> m_toolList;
    QList<HButton *> m_buttonList;
};

#endif // MAINTOOLGROUPWIDGET_H
