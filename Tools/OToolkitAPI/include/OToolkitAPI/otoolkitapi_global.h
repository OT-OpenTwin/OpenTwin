//! @file OToolkitAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(OTOOLKITAPI_LIB)
#  define OTOOLKITAPI_EXPORT Q_DECL_EXPORT
#  define OTOOLKITAPI_EXPORTONLY Q_DECL_EXPORT
# else
#  define OTOOLKITAPI_EXPORT Q_DECL_IMPORT
#  define OTOOLKITAPI_EXPORTONLY
# endif
#else
# define OTOOLKITAPI_EXPORT
# define OTOOLKITAPI_EXPORTONLY
#endif
