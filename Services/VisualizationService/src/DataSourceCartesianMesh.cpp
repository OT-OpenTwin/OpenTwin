// @otlicense
// File: DataSourceCartesianMesh.cpp
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

#include "DataSourceCartesianMesh.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityResultCartesianMeshVtk.h"

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

DataSourceCartesianMesh::DataSourceCartesianMesh()
{
	vtkGridAbs = vtkRectilinearGrid::New();
	vtkGridArg = vtkRectilinearGrid::New();
}

DataSourceCartesianMesh::~DataSourceCartesianMesh()
{
	FreeMemory();

	vtkGridAbs->Delete();
	vtkGridAbs = nullptr;

	vtkGridArg->Delete();
	vtkGridArg = nullptr;
}

bool DataSourceCartesianMesh::loadData(EntityBase* resultEntity, EntityBase* meshEntity)
{
	EntityResultCartesianMeshVtk* cartesianMesh = dynamic_cast<EntityResultCartesianMeshVtk *>(resultEntity);

	if (cartesianMesh != nullptr)
	{
		return loadData(cartesianMesh);
	}

	assert(0);
	return false;
}

bool DataSourceCartesianMesh::loadData(EntityResultCartesianMeshVtk* resultData)
{
	FreeMemory();

	assert(resultData != nullptr);

	std::string quantityName;
	EntityResultCartesianMeshVtk::eQuantityType quantityType = EntityResultCartesianMeshVtk::VECTOR_COMPLEX_MAG_PHASE;
	std::vector<char> dataAbs;
	std::vector<char> dataArg;

	resultData->getComplexData(quantityName, quantityType, dataAbs, dataArg);
	scaleFactor = resultData->getScaleFactor();

	if (dataAbs.empty() || dataArg.empty()) return false;

	vtkXMLRectilinearGridReader* gridReaderAbs = vtkXMLRectilinearGridReader::New();

	std::string inputAbs(dataAbs.data());
	gridReaderAbs->SetInputString(inputAbs);
	gridReaderAbs->ReadFromInputStringOn();
	gridReaderAbs->Update();

	vtkGridAbs->Delete();
	vtkGridAbs = gridReaderAbs->GetOutput();

	vtkXMLRectilinearGridReader* gridReaderArg = vtkXMLRectilinearGridReader::New();

	std::string inputArg(dataArg.data());
	gridReaderArg->SetInputString(inputArg);
	gridReaderArg->ReadFromInputStringOn();
	gridReaderArg->Update();

	vtkGridArg->Delete();
	vtkGridArg = gridReaderArg->GetOutput();

	// In the following, we assume that the structure of the abs and the arg data is identical. Therefore it is sufficient to chek the abs part only.
	for (int index = 0; index < vtkGridAbs->GetCellData()->GetNumberOfArrays(); index++)
	{
		std::string name = vtkGridAbs->GetCellData()->GetArrayName(index);

		if (name == quantityName)
		{
			if (quantityType == EntityResultCartesianMeshVtk::VECTOR_COMPLEX_MAG_PHASE)
			{
				vtkGridAbs->GetCellData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
				vtkGridArg->GetCellData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
				hasCellVector = true;
			}
			else
			{
				assert(0); // Unknown data type
			}
		}
	}

	for (int index = 0; index < vtkGridAbs->GetPointData()->GetNumberOfArrays(); index++)
	{
		std::string name = vtkGridAbs->GetPointData()->GetArrayName(index);

		if (name == quantityName)
		{
			if (quantityType == EntityResultCartesianMeshVtk::VECTOR_COMPLEX_MAG_PHASE)
			{
				vtkGridAbs->GetPointData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
				vtkGridArg->GetPointData()->SetActiveAttribute(index, vtkDataSetAttributes::VECTORS);
				hasPointVector = true;
			}
			else
			{
				assert(0); // Unknown data type
			}
		}
	}

	return true;
}

void DataSourceCartesianMesh::FreeMemory(void)
{
	vtkGridAbs->SetDimensions(0, 0, 0);

	vtkGridAbs->SetXCoordinates(nullptr);
	vtkGridAbs->SetYCoordinates(nullptr);
	vtkGridAbs->SetZCoordinates(nullptr);

	vtkGridAbs->GetPointData()->Initialize();
	vtkGridAbs->GetCellData()->Initialize();

	vtkGridArg->SetDimensions(0, 0, 0);

	vtkGridArg->SetXCoordinates(nullptr);
	vtkGridArg->SetYCoordinates(nullptr);
	vtkGridArg->SetZCoordinates(nullptr);

	vtkGridArg->GetPointData()->Initialize();
	vtkGridArg->GetCellData()->Initialize();

	hasPointScalar = false;
	hasCellScalar  = false;
	hasPointVector = false;
	hasCellVector  = false;
}

double DataSourceCartesianMesh::GetXMinCoordinate()
{
	return vtkGridAbs->GetBounds()[0] * scaleFactor;
}

double DataSourceCartesianMesh::GetYMinCoordinate()
{
	return vtkGridAbs->GetBounds()[2] * scaleFactor;
}

double DataSourceCartesianMesh::GetZMinCoordinate()
{
	return vtkGridAbs->GetBounds()[4] * scaleFactor;
}

double DataSourceCartesianMesh::GetXMaxCoordinate()
{
	return vtkGridAbs->GetBounds()[1] * scaleFactor;
}

double DataSourceCartesianMesh::GetYMaxCoordinate()
{
	return vtkGridAbs->GetBounds()[3] * scaleFactor;
}

double DataSourceCartesianMesh::GetZMaxCoordinate()
{
	return vtkGridAbs->GetBounds()[5] * scaleFactor;
}
