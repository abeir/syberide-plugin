#ifndef SYBERSDKUPDATE_GLOBAL_H
#define SYBERSDKUPDATE_GLOBAL_H

#include <QtGlobal>

#if defined(SYBERSDKUPDATE_LIBRARY)
#  define SYBERSDKUPDATESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SYBERSDKUPDATESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SYBERSDKUPDATE_GLOBAL_H