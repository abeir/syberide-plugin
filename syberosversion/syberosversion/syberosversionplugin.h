#ifndef SYBEROSVERSION_H
#define SYBEROSVERSION_H

#include "syberosversion_global.h"

#include <extensionsystem/iplugin.h>

namespace SyberOSVersion {
namespace Internal {

class SyberOSVersionPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "SyberOSVersion.json")

public:
    SyberOSVersionPlugin();
    ~SyberOSVersionPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void slotShowSDKVersion();
};

} // namespace Internal
} // namespace SyberOSVersion

#endif // SYBEROSVERSION_H
