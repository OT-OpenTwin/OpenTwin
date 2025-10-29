// @otlicense

#pragma once
#include "ResultFilter/ResultContainer.h"

class ResultContainerScalar : public ResultContainer
{
public:
	ResultContainerScalar(double * values, index_t size, int currentTimeStep);
	~ResultContainerScalar();
	double* GetResult() { return _result; };

	virtual void ApplyFilter(ResultFilter * filter) override;

protected:
	double * _result = nullptr;
};