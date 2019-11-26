#ifndef VERSIONWIDGET_H
#define VERSIONWIDGET_H

#include <QDialog>
#include<QMultiMap>


namespace Ui {
class VersionWidget;
}

class VersionWidget : public QDialog
{
    Q_OBJECT

public:
    explicit VersionWidget(QWidget *parent = 0);
    ~VersionWidget();

private slots:
    void slotShowCopyright(void);

private:
    void packageInfo(void);

private:
    Ui::VersionWidget *ui;
    QMultiMap<QString,QString> map;
};

#endif // VERSIONWIDGET_H
