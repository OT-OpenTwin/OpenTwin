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