//! @file OTClassHelper.h
//! @brief This file contains defines that may be used simplyfy class creation.
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <type_traits>

//! @def OT_DECL_NOCOPY
//! @brief Removes the default copy constructor and assignment operator.
#define OT_DECL_NOCOPY(___class) ___class(const ___class&) = delete; ___class& operator = (const ___class&) = delete;

//! @def OT_DECL_NOCOPY
//! @brief Declares the default copy constructor and assignment operator as default.
//! @warning Will change the current scope to public.
#define OT_DECL_DEFCOPY(___class) public: ___class(const ___class&) = default; ___class& operator = (const ___class&) = default;

//! @def OT_DECL_NOMOVE
//! @brief Removes the default move constructor and move operator.
#define OT_DECL_NOMOVE(___class) ___class(___class&&) = delete; ___class& operator = (___class&&) = delete;

//! @def OT_DECL_DEFMOVE
//! @brief Declares the default move constructor and move operator as default.
//! @warning Will change the current scope to public.
#define OT_DECL_DEFMOVE(___class) public: ___class(___class&&) noexcept = default; ___class& operator = (___class&&) noexcept = default;

//! @def OT_DECL_NODEFAULT
//! @brief Removes the default copy constructor and assignment operator.
#define OT_DECL_NODEFAULT(___class) ___class(void) = delete;

//! \def OT_DECL_STATICONLY
//! \ref OT_DECL_NODEFAULT
//! \ref OT_DECL_NOCOPY
//! \ref OT_DECL_NOMOVE
#define OT_DECL_STATICONLY(___class) OT_DECL_NODEFAULT(___class) OT_DECL_NOCOPY(___class) OT_DECL_NOMOVE(___class)

#if defined(OT_COMPILER_MSC)

//! \def OT_DECL_DEPRECATED
//! \brief Marks the method or class as deprecated.
#define OT_DECL_DEPRECATED(___message) __declspec(deprecated(___message))

#elif defined(OT_COMPILER_GNU) || defined(OT_COMPILER_CLANG)

//! \def OT_DECL_DEPRECATED
//! \brief Marks the method or class as deprecated.
#define OT_DECL_DEPRECATED(___message) __attribute__((deprecated))

#endif

//! @def OT_UNUSED
//! @brief Avoid "warning C4101: unreferenced local variable".
//! Use only when working with preprocessor definitions, unused locals should not occur in the first place :).
#define OT_UNUSED(___unusedVariable) (void)(___unusedVariable);
