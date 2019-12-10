#include <SyberosServiceCache>
#include "%SERVICEID%_workspace.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    return SYBEROS::SyberosServiceCache::qApplication(argc, argv, new %SERVICEID%_Workspace());
}
