#ifndef ONLINEINSTALLWIDGET_H
#define ONLINEINSTALLWIDGET_H
#include <QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QRadioButton>
#include<QCheckBox>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGroupBox>
#include<QGridLayout>
#include<QDialogButtonBox>
#include<QPushButton>
#include<QTimer>

class OnlineInstallWidget : public QWidget
{
    Q_OBJECT

public:
   explicit OnlineInstallWidget(QWidget *parent = 0);
  void createComponent();
  QVBoxLayout* createLayout();
   void initUI();
   void initConnect();

signals:
   void quitWindow();

public slots:
   void checkButtonEnable();

private:
    QLabel * sdkPathTextLabel;
    QLineEdit * sdkPathLineEdit;
    QRadioButton *onlyInstallSDKButton;
    QRadioButton *onlyInstallTargetButton;
    QRadioButton *InstallButton;
    QCheckBox *armv7CheckBox;
    QCheckBox *i686CheckBox;
    QCheckBox *main_devCheckBox;
    QCheckBox *xuanwuCheckBox;
    QCheckBox *miit_certCheckBox;
    QCheckBox *product_lilyCheckBox;
    QCheckBox *ginkgo_milletCheckBox;

    QGroupBox* archGroupBox;
    QGroupBox*targetBranchNameBox;
    QGroupBox *InstallChooseBox;
    QDialogButtonBox* buttonBox;
    QPushButton* installButton;
    QPushButton* cancelButton;
    QTimer *timer_check;

//    QCheckBox *xuanwu_stableCheckBox;
};

#endif // HOMEWIDGET_H
