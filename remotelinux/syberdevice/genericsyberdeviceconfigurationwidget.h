#ifndef REMOTELINUX_GENERICSYBERDEVICECONFIGURATIONWIDGET_H
#define REMOTELINUX_GENERICSYBERDEVICECONFIGURATIONWIDGET_H

#include <projectexplorer/devicesupport/idevicewidget.h>

#include "remotelinux_export.h"

namespace RemoteLinux {

namespace Ui { class GenericSyberDeviceConfigurationWidget; }

class REMOTELINUX_EXPORT GenericSyberDeviceConfigurationWidget
        : public ProjectExplorer::IDeviceWidget
{
    Q_OBJECT

public:
    explicit GenericSyberDeviceConfigurationWidget(
        const ProjectExplorer::IDevice::Ptr &deviceConfig, QWidget *parent = 0);
    ~GenericSyberDeviceConfigurationWidget();

private slots:
    void authenticationTypeChanged();
    void hostNameEditingFinished();
    void sshPortEditingFinished();
    void timeoutEditingFinished();
    void userNameEditingFinished();
    void osVersionSelectionChanged();
    void passwordEditingFinished();
    void keyFileEditingFinished();
    void gdbServerEditingFinished();
    void showPassword(bool showClearText);
    void handleFreePortsChanged();
    void setPrivateKey(const QString &path);
    void createNewKey();
    void hostKeyCheckingChanged(bool doCheck);

private:
    void updateDeviceFromUi();
    void updatePortsWarningLabel();
    void initGui();

    Ui::GenericSyberDeviceConfigurationWidget *m_ui;
};

} // namespace RemoteLinux

#endif // REMOTELINUX_GENERICSYBERDEVICECONFIGURATIONWIDGET_H
