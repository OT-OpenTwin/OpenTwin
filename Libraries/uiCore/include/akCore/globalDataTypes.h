/*
 *	File:		globalDataTypes.h
 *	Package:	akCore
 *
 *  Created on: September 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include <QtCore/qglobal.h>

#if defined(UICORE_EXPORT) // inside DLL
#   define UICORE_API_EXPORT   Q_DECL_EXPORT
#else // outside DLL
#   define UICORE_API_EXPORT   Q_DECL_IMPORT
#endif  // LIBRARY_EXPORT

 // WINDOWS
#if defined(_WIN32) || defined(_WIN64)
//! The current OS is windows
#define AK_OS_WINDOWS
#endif
// UNIX
#if defined(unix) || defined(__unix) || defined (__unix__)
//! The current OS is Unix
#define AK_OS_UNIX
#endif
// APPLE
#if defined(__APPLE__) || defined(__MACH__)
//! The current OS is Mac
#define AK_OS_MAC
#endif
// LINUX
#if defined(__linux__) || defined(linux) || defined(__linux)
//! The current OS is Linux
#define AK_OS_LINUX
#endif
// FreeBSD
#if defined (__FreeBSD__)
//! The current OS is FreeBSD
#define AK_OS_FreeBSD
#endif
// ANDROID
#if defined(__ANDROID__)
//! The current OS is Android
#define AK_OS_ANDROID
#endif

#if defined (AK_OS_WINDOWS)
#define AK_CHAR_NL '\n'
#define AK_CSTR_NL "\n"
#else
#define AK_CHAR_NL '\n'
#define AK_CSTR_NL "\n"
#endif

namespace ak {

	//! @brief The UID datatype used for objects
	typedef unsigned long long UID;

	//! @brief The ID datatype used for items
	typedef int	ID;

	//! @brief Represents the invalid UID for an object
	const UID invalidUID = 0;

	//! @brief Represents the invalid ID for an item
	const ID invalidID = -1;

}
