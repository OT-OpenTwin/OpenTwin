/*
 *	File:		rbeAssert.h
 *	Package:	rbeCore
 *
 *  Created on: November 12, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include <cassert>

#ifdef _DEBUG
#define rbeAssert(___expression, ___message) (void)((!!(___expression)) || (_wassert(_CRT_WIDE(#___expression) L"\n\n" _CRT_WIDE(___message), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))
#else
#define rbeAssert(___expression, ___message)
#endif // _DEBUG