#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/devicesupport/devicemanagermodel.h>
#include <projectexplorer/devicesupport/idevice.h>

#include <QWidget>

//namespace CovManager {
//    class SshHandler;
//}

namespace SyberOSDevice {
namespace Internal {
class SshHandler;

QT_BEGIN_NAMESPACE
namespace Ui { class MonitorWidget; }
QT_END_NAMESPACE

class MonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

    void destroySsh();
    void addTab(QWidget* page, QString pageName);
    void init();
private:
    void printScreen();
    void connectSsh();
private slots:
    void initSsh(int index);


private:
    Ui::MonitorWidget        *ui;
    SshHandler              *m_sshHandler = nullptr;
    QList<QPoint>             m_posList;
    QString                 m_strFormat;
    QString                 m_strSeperator;
    ProjectExplorer::DeviceManager * const m_deviceManager;
    ProjectExplorer::DeviceManagerModel * const m_deviceManagerModel;

                        //m_devicePtr;
};
}
}

#endif // CUSTOMWIDGET_H
