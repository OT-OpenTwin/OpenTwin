#include "SystemDependencies\SystemProperties.h"
#include <thread>

void SystemProperties::SetMaximumNbOfThreads()
{
#ifdef _OPENMP
	_maximumNbOfThreads = omp_get_max_threads();
	omp_set_num_threads(_maximumNbOfThreads);
#else
	_maximumNbOfThreads = 1;
#endif // _OPENMP
}

const int SystemProperties::GetActiveNbOfThreads() const
{
#ifdef _OPENMP
	return omp_get_num_threads();
#else
	return 1;
#endif // _OPENMP
}
