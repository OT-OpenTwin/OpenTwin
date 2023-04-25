#pragma once
#include "ResultFilter/ResultFilter.h"

class ResultContainer
{
public:
	ResultContainer(int currentTimeStep, index_t size) : _currentTimeStep(currentTimeStep), _size(size) {};
	virtual void ApplyFilter(ResultFilter * filter) = 0;
	
	const int GetCurrentTimestep(void) { return _currentTimeStep; }
	const index_t GetSize(void) { return _size; }
protected:
	index_t _size;
	int _currentTimeStep;
};