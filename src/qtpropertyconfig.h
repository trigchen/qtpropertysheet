#pragma once

#ifdef QTPROPERTYSHEET_STATIC
#       define QTPROPERTYSHEET_DLL
#else

#include <QtGlobal>
#ifdef QTPROPERTYSHEET_SHARED
#       define QTPROPERTYSHEET_DLL Q_DECL_EXPORT
#else
#       define QTPROPERTYSHEET_DLL Q_DECL_IMPORT
#endif

#endif
