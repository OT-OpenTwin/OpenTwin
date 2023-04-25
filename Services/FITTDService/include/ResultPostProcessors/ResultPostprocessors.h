#pragma once
#include "SystemDependencies/SystemDependentDefines.h"

class ResultPostProcessor
{
public:
	virtual void PerformProstprocessing(double * result, index_t size) = 0;
};