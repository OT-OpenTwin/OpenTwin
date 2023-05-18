#pragma once

// std header
#include <cassert>

#ifdef _DEBUG
//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define otAssert(___expression, ___message) (void)((!!(___expression)) || (_wassert(_CRT_WIDE(#___expression) L"\n\n" _CRT_WIDE(___message), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))

//! @brief Check if the provided pointer is not a nullptr
#define OTAssertNullptr(___ptr) (void)((!!(___ptr != nullptr)) || (_wassert(_CRT_WIDE(#___ptr) _CRT_WIDE("\n\n[Caution]\nnullptr for \"") _CRT_WIDE(#___ptr) _CRT_WIDE("\"provided"), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))

#else

//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define otAssert(___expression, ___message)

//! @brief Check if the provided pointer is not a nullptr
#define OTAssertNullptr(___ptr)

#endif // _DEBUG