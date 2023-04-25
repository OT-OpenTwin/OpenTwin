#include "ResultHandling/ResultSinkFilePrinter.h"


ResulSinkFilePrinter::ResulSinkFilePrinter(int executionFrequency, int totalTimeSteps, std::string path, std::string headLine) : ResultSink(executionFrequency, totalTimeSteps)
{
	_myfile.open(path);
	_myfile << headLine<< std::endl;
}

ResulSinkFilePrinter::~ResulSinkFilePrinter()
{
	_myfile.close();
}

void ResulSinkFilePrinter::ConsumeResultContainer(ResultContainer * container)
{
	auto scalarContainer = dynamic_cast<ResultContainerScalar*>(container);
	if (scalarContainer != nullptr)
	{
		PrintScalarfield(scalarContainer);
	}
	else
	{
		auto vectorContainer = dynamic_cast<ResultContainerVector3DComplex*>(container);
		if (vectorContainer == nullptr)
		{
			throw std::invalid_argument("File printer got an unsupported result container type.");
		}
		PrintVectorfield(vectorContainer);
	}
}

void ResulSinkFilePrinter::PerformPostProcessing()
{

}

void ResulSinkFilePrinter::PrintVectorfield(ResultContainerVector3DComplex * vectorField)
{

	index_t size = vectorField->GetSize();
	_myfile << "X-Component:\n";
	for (int i = 0; i < size; i++)
	{
		_myfile << vectorField->GetRealComponentX()[i] << "\n";
	}
	_myfile << "\nY-Component:\n";
	for (int i = 0; i < size; i++)
	{
		_myfile << vectorField->GetRealComponentY()[i] << "\n";
	}
	_myfile << "\nZ-Component:\n";
	for (int i = 0; i < size; i++)
	{
		_myfile << vectorField->GetRealComponentZ()[i] << "\n";
	}
}

void ResulSinkFilePrinter::PrintScalarfield(ResultContainerScalar * scalarField)
{
	index_t size = scalarField->GetSize();
	for (int i = 0; i < size; i++)
	{
		_myfile << scalarField->GetResult()[i] << "\n";
	}

}
