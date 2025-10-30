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

#include <string>

#include "DataSourceManagerItem.h"

#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>

class EntityResultUnstructuredMesh;
class EntityResultUnstructuredMeshData;
class EntityResultUnstructuredMeshVtk;

class DataSourceUnstructuredMesh : public DataSourceManagerItem
{
public:
	DataSourceUnstructuredMesh();
	virtual ~DataSourceUnstructuredMesh();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkUnstructuredGrid* GetVtkGrid() { return vtkGrid; };

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

private:
	bool loadData(EntityResultUnstructuredMeshData* resultEntity, EntityBase* meshEntity);
	bool loadData(EntityResultUnstructuredMeshVtk* resultData);

	bool loadMeshData(EntityBase* meshEntity);
	bool loadResultData(EntityBase* resultEntity);
	void FreeMemory();
	void buildScalarArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);
	void buildVectorArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);

	vtkUnstructuredGrid *vtkGrid;

	bool hasPointScalar = false;
	bool hasCellScalar = false;
	bool hasPointVector = false;
	bool hasCellVector = false;
};
