// @otlicense
// File: VtkDriverUnstructuredVectorVolume.h
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
#include <ctime>
#include <vector>
#include <list>
#include <vtkAlgorithmOutput.h>
#include <vtkDataSetMapper.h>

#include "vtkRectilinearGrid.h"
#include "VtkDriver.h"
#include "EntityResultBase.h"
#include "PropertyBundleDataHandlePlane.h"
#include "PropertyBundleDataHandleScaling.h"
#include "PropertyBundleDataHandleVisUnstructuredVector.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredVectorVolume : public VtkDriver {
public:
	VtkDriverUnstructuredVectorVolume();
	virtual ~VtkDriverUnstructuredVectorVolume();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;
	
private:
	PropertyBundleDataHandlePlane * planeData = nullptr;
	PropertyBundleDataHandleScaling * scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredVector * visData = nullptr;
	double * scalarRange = nullptr;

	vtkAlgorithmOutput* ApplyCutplane(osg::Node *parent);
	void Assemble2DNode(osg::Node *parent);
	void Assemble3DNode(osg::Node* parent);

	void AddNodeVectors(vtkAlgorithmOutput* input, osg::Node* parent);
	vtkAlgorithmOutput* SetScalarValues(void);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetArrowSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;

	std::list<vtkObject*> objectsToDelete;
};