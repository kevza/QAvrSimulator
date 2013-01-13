#ifndef AVR_CORE_GLOBAL_H
#define AVR_CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AVR_CORE_LIBRARY)
#  define AVR_CORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define AVR_CORESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AVR_CORE_GLOBAL_H
