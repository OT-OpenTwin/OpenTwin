// @otlicense
// File: DataSourceResult3D.h
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

#pragma once
#include "DataSourceManagerItem.h"
#include "EntityCompressedVector.h"
#include "EntityMeshCartesianData.h"
#include "EntityResultBase.h"
#include "EntityResult3DData.h"

#include <vector>
#include <vtkRectilinearGrid.h>
#include <vtkNew.h>

class DataSourceResult3D : public DataSourceManagerItem, public EntityResultBase
{
public:
	DataSourceResult3D();
	virtual ~DataSourceResult3D();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkRectilinearGrid * GetVtkGrid() { return vtkGrid.GetPointer(); };
		
	double GetXMinCoordinate() { return _xMinCoordinate; }
	double GetYMinCoordinate() { return _yMinCoordinate; }
	double GetZMinCoordinate() { return _zMinCoordinate; }
	double GetXMaxCoordinate() { return _xMaxCoordinate; }
	double GetYMaxCoordinate() { return _yMaxCoordinate; }
	double GetZMaxCoordinate() { return _zMaxCoordinate; }

private:
	EntityResult3DData * entityResultData3D = nullptr;
	vtkNew<vtkRectilinearGrid> vtkGrid;
	
	void SetGridValues(const double * xComponent, const double * yComponent, const double * zComponent, size_t size);
	void SetGridDimensions(EntityMeshCartesianData * meshData);
	const double FindMinimumDiscretization(std::vector<double> & coordinates);
	void FreeMemory(void);

	double _xMinCoordinate;
	double _yMinCoordinate;
	double _zMinCoordinate;
	double _xMaxCoordinate;
	double _yMaxCoordinate;
	double _zMaxCoordinate;
		
};