// @otlicense
// File: OTAssert.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// std header
#include <cassert>

#ifdef _DEBUG
//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define OTAssert(___expression, ___message) (void)((!!(___expression)) || (_wassert(_CRT_WIDE(#___expression) L"\n\n" _CRT_WIDE(___message), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))

//! @brief Check if the provided pointer is not a nullptr
#define OTAssertNullptr(___ptr) (void)((!!(___ptr != nullptr)) || (_wassert(_CRT_WIDE(#___ptr) _CRT_WIDE("\n\n[Caution]\nnullptr for \"") _CRT_WIDE(#___ptr) _CRT_WIDE("\"provided"), _CRT_WIDE(__FILE__), (unsigned)__LINE__), 0))

#else

//! @brief Adds the ability to provide a custom text to the cassert macro (assertion only in debug mode)
#define OTAssert(___expression, ___message)

//! @brief Check if the provided pointer is not a nullptr
#define OTAssertNullptr(___ptr)

#endif // _DEBUG