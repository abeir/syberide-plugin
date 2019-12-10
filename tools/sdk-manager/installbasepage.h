#ifndef INSTALLBASEPAGE_H
#define INSTALLBASEPAGE_H

#include <QWizardPage>
#include<QString>
#include<QMultiMap>

class InstallBasePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit InstallBasePage(QWidget *parent = 0);
    void setColoredTitle(const QString &title);
    void setColoredSubTitle(const QString &subTitle);
    QString productName() const;
    virtual bool isComplete() const;
    void setComplete(bool complete);
   // virtual void	initializePage(){};
    virtual void entering(){}
    virtual void leaving(){}

protected:
    static QMultiMap<QString,QString> selectionData;
    static QString sdkPathTip;

signals:

public slots:

private:
    QString m_titleColor;
    bool m_complete;


};

#endif // INSTALLBASEPAGE_H
