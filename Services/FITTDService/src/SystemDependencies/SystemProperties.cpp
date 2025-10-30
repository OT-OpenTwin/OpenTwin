// @otlicense
// File: SystemProperties.cpp
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
