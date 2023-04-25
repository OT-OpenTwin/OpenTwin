#include "ResultPostProcessors/ResultPostProcessingComplexNormalization.h"

ResultPostProcessingComplexNormalization::ResultPostProcessingComplexNormalization(ResultSinkScalarComplexSum * normalizer) : _normalizer(normalizer)
{}

void ResultPostProcessingComplexNormalization::PerformProstprocessing(double * result, index_t size)
{
	auto normalizerResult = _normalizer->GetResultContainer();
	double realDivisor = *normalizerResult->GetResult();
	double imagDivisor = *normalizerResult->GetResultImag();
	if (realDivisor == 0.)
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i] = 0;
		}
	}
	else
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i] /= realDivisor;
		}
	}
	if (imagDivisor == 0.)
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i + size] = 0;
		}
	}
	else
	{
		for (index_t i = 0; i < size; i++)
		{
			result[i + size] /= imagDivisor;
		}
	}
	
}
