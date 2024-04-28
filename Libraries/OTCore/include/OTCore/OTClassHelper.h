//! @file OTClassHelper.h
//! @brief This file contains defines that may be used simplyfy class creation.
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

//! @brief Removes the default copy constructor and assignment operator
#define OT_DECL_NOCOPY(___class) ___class(const ___class&) = delete; ___class& operator = (const ___class&) = delete;

//! @brief Removes the default move constructor and move operator
#define OT_DECL_NOMOVE(___class) ___class(const ___class&&) = delete; ___class& operator = (const ___class&&) = delete;

//! @brief Removes the default copy constructor and assignment operator
#define OT_DECL_NODEFAULT(___class) ___class(void) = delete;