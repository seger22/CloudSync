#ifndef LIBSHA1_GLOBAL_H
#define LIBSHA1_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBSHA1_LIBRARY)
#  define LIBSHA1SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHA1SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBSHA1_GLOBAL_H
