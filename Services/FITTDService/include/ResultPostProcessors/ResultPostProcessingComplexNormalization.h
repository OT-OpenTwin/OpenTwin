// @otlicense

#pragma once

#include "ResultPostProcessors/ResultPostprocessors.h"
#include "ResultFilter/ResultSinkScalarComplexSum.h"
class ResultPostProcessingComplexNormalization : public ResultPostProcessor
{
public:
	ResultPostProcessingComplexNormalization(ResultSinkScalarComplexSum* normalizer);
	void PerformProstprocessing(double * result, index_t size) override;

private:
	ResultSinkScalarComplexSum * _normalizer;
};