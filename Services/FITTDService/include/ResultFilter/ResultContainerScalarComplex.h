#pragma once
#include "ResultFilter/ResultContainerScalar.h"

class ResultContainerScalarComplex : public ResultContainerScalar
{
public:
	ResultContainerScalarComplex(double * values, index_t size, int currentTimestep) : ResultContainerScalar(values, size, currentTimestep) {};
	double * GetResultImag() { return &_result[_size]; };
};