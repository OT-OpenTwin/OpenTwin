// @otlicense
// File: DataSourceUnstructuredMesh.h
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

#include "VisualizationServiceTypes.h"
#include "DataSourceManagerItem.h"

#include <vtkRectilinearGrid.h>
#include <vtkNew.h>

#include <string>

class EntityResultCartesianMeshVtk;

class DataSourceCartesianMesh : public DataSourceManagerItem
{
public:
	DataSourceCartesianMesh();
	virtual ~DataSourceCartesianMesh();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkRectilinearGrid* GetVtkGridAbs() { return vtkGridAbs; };
	vtkRectilinearGrid* GetVtkGridArg() { return vtkGridArg; };

	double GetXMinCoordinate();
	double GetYMinCoordinate();
	double GetZMinCoordinate();
	double GetXMaxCoordinate();
	double GetYMaxCoordinate();
	double GetZMaxCoordinate();

	bool GetHasPointScalar() { return hasPointScalar; }
	bool GetHasPointVector() { return hasPointVector; }
	bool GetHasCellScalar() { return hasCellScalar; }
	bool GetHasCellVector() { return hasCellVector; }

	double getScaleFactor() { return scaleFactor; }

private:
	bool loadData(EntityResultCartesianMeshVtk* resultData);

	void FreeMemory();

	vtkRectilinearGrid* vtkGridAbs;
	vtkRectilinearGrid* vtkGridArg;

	double scaleFactor = 1.0;

	bool hasPointScalar = false;
	bool hasCellScalar = false;
	bool hasPointVector = false;
	bool hasCellVector = false;
};
