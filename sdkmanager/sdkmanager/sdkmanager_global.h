#ifndef SDKMANAGER_GLOBAL_H
#define SDKMANAGER_GLOBAL_H

#include <QtGlobal>

#if defined(SDKMANAGER_LIBRARY)
#  define SDKMANAGERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SDKMANAGERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SDKMANAGER_GLOBAL_H
