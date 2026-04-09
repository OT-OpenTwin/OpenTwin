// @otlicense
// File: DataSourceVtkTime.cpp
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

#include "DataSourceVtkTime.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityResultVtkTime.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkTetra.h>

DataSourceVtkTime::DataSourceVtkTime()
{
	vtkGrid = vtkRectilinearGrid::New();
}

DataSourceVtkTime::~DataSourceVtkTime()
{
	FreeMemory();

	vtkGrid->Delete();
	vtkGrid = nullptr;
}

bool DataSourceVtkTime::loadData(EntityBase* resultEntity, EntityBase* meshEntity)
{
	EntityResultVtkTime* cartesianMesh = dynamic_cast<EntityResultVtkTime*>(resultEntity);

	if (cartesianMesh != nullptr)
	{
		return loadData(cartesianMesh);
	}

	assert(0);
	return false;
}

bool DataSourceVtkTime::loadData(EntityResultVtkTime* resultData)
{
	FreeMemory();

	assert(resultData != nullptr);

	double time = 0.0;

	std::string quantityName;
	std::vector<char> data;

	resultData->getTimeData(time, quantityName, data);
	scaleFactor = resultData->getScaleFactor();

	if (data.empty()) return false;

	vtkXMLRectilinearGridReader* gridReader = vtkXMLRectilinearGridReader::New();

	std::string inputAbs(data.data());
	gridReader->SetInputString(inputAbs);
	gridReader->ReadFromInputStringOn();
	gridReader->Update();

	vtkGrid->Delete();
	vtkGrid = gridReader->GetOutput();

	for (int index = 0; index < vtkGrid->GetCellData()->GetNumberOfArrays(); index++)
	{
		std::string name = vtkGrid->GetCellData()->GetArrayName(index);

		if (name == quantityName)
		{
			vtkGrid->GetCellData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
			hasCellVector = true;
		}
	}

	for (int index = 0; index < vtkGrid->GetPointData()->GetNumberOfArrays(); index++)
	{
		std::string name = vtkGrid->GetPointData()->GetArrayName(index);

		if (name == quantityName)
		{
			vtkGrid->GetPointData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
			hasPointVector = true;
		}
	}

	return true;
}

void DataSourceVtkTime::FreeMemory(void)
{
	vtkGrid->SetDimensions(0, 0, 0);

	vtkGrid->SetXCoordinates(nullptr);
	vtkGrid->SetYCoordinates(nullptr);
	vtkGrid->SetZCoordinates(nullptr);

	vtkGrid->GetPointData()->Initialize();
	vtkGrid->GetCellData()->Initialize();

	hasPointScalar = false;
	hasCellScalar  = false;
	hasPointVector = false;
	hasCellVector  = false;
}

double DataSourceVtkTime::GetXMinCoordinate()
{
	return vtkGrid->GetBounds()[0] * scaleFactor;
}

double DataSourceVtkTime::GetYMinCoordinate()
{
	return vtkGrid->GetBounds()[2] * scaleFactor;
}

double DataSourceVtkTime::GetZMinCoordinate()
{
	return vtkGrid->GetBounds()[4] * scaleFactor;
}

double DataSourceVtkTime::GetXMaxCoordinate()
{
	return vtkGrid->GetBounds()[1] * scaleFactor;
}

double DataSourceVtkTime::GetYMaxCoordinate()
{
	return vtkGrid->GetBounds()[3] * scaleFactor;
}

double DataSourceVtkTime::GetZMaxCoordinate()
{
	return vtkGrid->GetBounds()[5] * scaleFactor;
}
