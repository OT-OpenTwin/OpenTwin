// @otlicense
// File: DataSourceCartesianVector.cpp
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


#include "stdafx.h"

#include "DataSourceCartesianVector.h"
#include "EntityBase.h"
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>


DataSourceCartesianVector::DataSourceCartesianVector() : 
	resultData(nullptr),
	meshData(nullptr)
{

}

DataSourceCartesianVector::~DataSourceCartesianVector()
{
	if (resultData != nullptr)
	{
		delete resultData;
		resultData = nullptr;
	}

	if (meshData != nullptr)
	{
		delete meshData;
		meshData = nullptr;
	}
}

bool DataSourceCartesianVector::loadData(EntityBase *resultEntity, EntityBase *meshEntity)
{
	assert(resultData == nullptr);
	assert(meshData == nullptr);

	// First, load the data from the data base
	resultData = dynamic_cast<EntityCartesianVector *>(resultEntity);
	if (resultData == nullptr)
	{
		assert(0); // Wrong data type
		return false;
	}

	meshData = dynamic_cast<EntityMeshCartesianData*>(meshEntity);

	if (meshData == nullptr)
	{
		delete resultData;
		resultData = nullptr;

		assert(0); // Unable to read the mesh data
		return false;
	}

	return true; // successful loading
}

