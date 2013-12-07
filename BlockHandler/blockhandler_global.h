#ifndef BLOCKHANDLER_GLOBAL_H
#define BLOCKHANDLER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BLOCKHANDLER_LIBRARY)
#  define BLOCKHANDLERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BLOCKHANDLERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BLOCKHANDLER_GLOBAL_H