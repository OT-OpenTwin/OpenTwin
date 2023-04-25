#pragma once
#include <stdint.h>

#ifdef _OPENMP /*Check OpenMP usage*/

#if _OPENMP >= 201307 /*OpenMP Version Check*/
	typedef uint64_t index_t;
#else
	typedef int64_t index_t;
#endif /*OpenMP Version Check*/

#else
typedef int64_t index_t;
#endif/*Check OpenMP usage*/
