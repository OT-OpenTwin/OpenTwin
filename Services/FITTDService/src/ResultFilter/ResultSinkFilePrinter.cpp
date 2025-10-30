// @otlicense
// File: ResultSinkFilePrinter.cpp
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
