//! @file OTClassHelper.h
//! @brief This file contains defines that may be used simplyfy class creation.
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include <OTSystem/ArchitectureInfo.h>

//! \def OT_DECL_NOCOPY
//! \brief Removes the default copy constructor and assignment operator
#define OT_DECL_NOCOPY(___class) ___class(const ___class&) = delete; ___class& operator = (const ___class&) = delete;

//! \def OT_DECL_NOMOVE
//! \brief Removes the default move constructor and move operator
#define OT_DECL_NOMOVE(___class) ___class(const ___class&&) = delete; ___class& operator = (const ___class&&) = delete;

//! \def OT_DECL_NODEFAULT
//! \brief Removes the default copy constructor and assignment operator
#define OT_DECL_NODEFAULT(___class) ___class(void) = delete;


#if defined(OT_COMPILER_MSC)

//! \def OT_DECL_DEPRECATED
//! \brief Marks the method or class as deprecated.
#define OT_DECL_DEPRECATED(___message) __declspec(deprecated(___message))

#elif defined(OT_COMPILER_GNU) || defined(OT_COMPILER_CLANG)

//! \def OT_DECL_DEPRECATED
//! \brief Marks the method or class as deprecated.
#define OT_DECL_DEPRECATED(___message) __attribute__((deprecated))

#endif