#ifndef MAINTOPWIDGET_H
#define MAINTOPWIDGET_H

#include <QWidget>
#include<QLabel>

class QPushButton;

class MainTopWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainTopWidget(QWidget *parent = 0);
    /*!
     * \brief turnPage
     *
     * \param page
     */
    void setMenuDown(bool isDown);

signals:
    void turnPageSignal(int);
    void showMenu(QPoint);
    void minMain();
    void closeMain();
protected:
    void paintEvent(QPaintEvent *);
private slots:
    void showMenuSlot();
private:
    void initUI();
    void initConnect();
private:
    QLabel *m_logoLabel;
    QPushButton *m_menuButton;
    QPushButton *m_minButton;
    QPushButton *m_closeButton;
};

#endif // MAINTOPWIDGET_H
