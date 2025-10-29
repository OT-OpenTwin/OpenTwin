// @otlicense

#pragma once

#ifdef _OPENMP
#include <omp.h>
#endif

#include "SystemDependencies/InstructionSet.h"

class SystemProperties
{
	InstructionSet _instructionSet;
	int _maximumNbOfThreads = 1;
	
	void SetMaximumNbOfThreads();
	SystemProperties() 
	{
		SetMaximumNbOfThreads();
	};
public:

	const bool CPUSupportsAVX2() const { return _instructionSet.AVX2(); };
	const bool CPUSupportsAVX512() const { return _instructionSet.AVX512F(); };
	const int GetMaximumNbOfThreads() const { return _maximumNbOfThreads; };
	const int GetActiveNbOfThreads() const;



	static const SystemProperties & GetInstance()
	{
		static SystemProperties INSTANCE;
		return INSTANCE; 
	} 
};