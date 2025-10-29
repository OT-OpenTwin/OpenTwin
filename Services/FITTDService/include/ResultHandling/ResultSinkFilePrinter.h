// @otlicense

#pragma once
#include "ResultFilter/ResultSink.h"
#include "ResultFilter/ResultContainerScalar.h"
#include "ResultFilter/ResultContainerVector3DComplex.h"

#include <string>
#include <fstream>

class ResulSinkFilePrinter : public ResultSink
{
public:
	ResulSinkFilePrinter(int executionFrequency, int totalTimeSteps, std::string path, std::string headLine);
	~ResulSinkFilePrinter();
	virtual void ConsumeResultContainer(ResultContainer * container) override;
	virtual void PerformPostProcessing() override;

private:
	std::ofstream _myfile;
	void PrintVectorfield(ResultContainerVector3DComplex* vectorField);
	void PrintScalarfield(ResultContainerScalar* scalarField);

};
