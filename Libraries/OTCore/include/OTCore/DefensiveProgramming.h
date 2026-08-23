// @otlicense
// File: DefensiveProgramming.h
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

// OpenTwin header
#include "OTSystem/OTAssert.h"

#ifdef _DEBUG		
#define INVARIANT_ASSERT(condition){ \
	if (!(condition))		         \
	{						         \
		OTAssert(false);             \
	}						         \
	}						
#else
#define INVARIANT_ASSERT(condition)
#endif // _DEBUG

//! @brief Check the invariant of a class
//! The invariant method of a class shall be protected and inline.
//! It should be executed before and after a method is executed.
#define INVARIANT INVARIANT_ASSERT(invariant())

#define POST(condition){		 \
	INVARIANT_ASSERT(condition); \
	INVARIANT;					 \
}								

#define PRE(condition){			 \
	INVARIANT;					 \
	INVARIANT_ASSERT(condition); \
}