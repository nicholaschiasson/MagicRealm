#ifndef COMMONLIBRARY_GLOBAL_H
#define COMMONLIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef COMMONLIBRARY_LIB
# define COMMONLIBRARY_EXPORT Q_DECL_EXPORT
#else
# define COMMONLIBRARY_EXPORT Q_DECL_IMPORT
#endif

#endif // COMMONLIBRARY_GLOBAL_H