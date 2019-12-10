#ifndef REMOTESYBERRUNCONFIGURATION_H
#define REMOTESYBERRUNCONFIGURATION_H

#include "remotelinux_export.h"

#include <projectexplorer/runconfiguration.h>

#include <QStringList>

namespace Utils {
class Environment;
class PortList;
}

namespace RemoteLinux {
class RemoteSyberRunConfigurationWidget;
class RemoteLinuxDeployConfiguration;

namespace Internal {
class RemoteSyberRunConfigurationPrivate;
class RemoteSyberRunConfigurationFactory;
} // namespace Internal

class REMOTELINUX_EXPORT RemoteSyberRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    Q_DISABLE_COPY(RemoteSyberRunConfiguration)
    friend class Internal::RemoteSyberRunConfigurationFactory;
    friend class RemoteSyberRunConfigurationWidget;

public:
    RemoteSyberRunConfiguration(ProjectExplorer::Target *parent, Core::Id id,
                                const QString &targetName);
    ~RemoteSyberRunConfiguration() override;

    bool isEnabled() const override;
    QWidget *createConfigurationWidget() override;
    Utils::OutputFormatter *createOutputFormatter() const override;

    ProjectExplorer::Runnable runnable() const override;

    QString localExecutableFilePath() const;
    QString defaultRemoteExecutableFilePath() const;
    QString remoteExecutableFilePath() const;
    QString arguments() const;
    void setArguments(const QString &args);
    QString workingDirectory() const;
    void setWorkingDirectory(const QString &wd);
    void setAlternateRemoteExecutable(const QString &exe);
    QString alternateRemoteExecutable() const;
    void setUseAlternateExecutable(bool useAlternate);
    bool useAlternateExecutable() const;

    QVariantMap toMap() const override;

    static const char *IdPrefix;

signals:
    void deploySpecsChanged();
    void targetInformationChanged() const;

protected:
    RemoteSyberRunConfiguration(ProjectExplorer::Target *parent,
        RemoteSyberRunConfiguration *source);
    bool fromMap(const QVariantMap &map) override;
    QString defaultDisplayName();

protected slots:
    void updateEnabledState() { emit enabledChanged(); }

private slots:
    void handleBuildSystemDataUpdated();

private:
    void init();

    Internal::RemoteSyberRunConfigurationPrivate * const d;
};

} // namespace RemoteLinux

#endif // REMOTESYBERRUNCONFIGURATION_H
