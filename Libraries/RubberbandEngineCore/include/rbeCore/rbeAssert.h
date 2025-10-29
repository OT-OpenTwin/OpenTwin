// @otlicense

#pragma once

#include <cassert>

#ifdef _DEBUG
#define rbeAssert(___expression, ___message) (void)((!!(___expression)) || (_wassert(_CRT_WIDE(#___expression) L"\n\n" _CRT_WIDE(___message), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))
#else
#define rbeAssert(___expression, ___message)
#endif // _DEBUG