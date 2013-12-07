#ifndef ZDELTA_GLOBAL_H
#define ZDELTA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZDELTA_LIBRARY)
#  define ZDELTASHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZDELTASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZDELTA_GLOBAL_H
