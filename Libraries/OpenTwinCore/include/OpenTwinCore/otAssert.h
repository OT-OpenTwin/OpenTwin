#pragma once

// std header
#include <cassert>

#ifdef _DEBUG
//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define otAssert(___expression, ___message) (void)((!!(___expression)) || (_wassert(_CRT_WIDE(#___expression) L"\n\n" _CRT_WIDE(___message), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))

#else
//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define otAssert(___expression, ___message)
#endif // _DEBUG