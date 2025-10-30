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
#include <cassert>

#ifdef _DEBUG		
#define ASSERT(condition){	\
	if (!(condition))		\
	{						\
		assert(false);		\
	}						\
	}						
#else
#define ASSERT(condition)
#endif // _DEBUG

	// Checks the invariant method of a class.
	// Method shall be protected and inline.
	// Should be executed before and after a method is executed.
#define INVARIANT ASSERT(invariant())

#define POST(condition){		\
	ASSERT(condition);			\
	INVARIANT;					\
}								

#define PRE(condition){			\
	INVARIANT;					\
	ASSERT(condition);			\
}