#ifndef SSNETWORK_GLOBAL_H
#define SSNETWORK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SSNETWORK_LIBRARY)
#  define SSNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SSNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SSNETWORK_GLOBAL_H
