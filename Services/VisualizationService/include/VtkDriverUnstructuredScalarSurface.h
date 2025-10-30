// @otlicense
// File: VtkDriverUnstructuredScalarSurface.h
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

#include "VtkDriver.h"
#include "EntityResultBase.h"
#include "PropertyBundleDataHandleScaling.h"
#include "PropertyBundleDataHandleVisUnstructuredScalarSurface.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredScalarSurface : public VtkDriver {
public:
	VtkDriverUnstructuredScalarSurface();
	virtual ~VtkDriverUnstructuredScalarSurface();

	virtual void setProperties(EntityVis2D3D* visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem* dataItem) override;

private:
	PropertyBundleDataHandleScaling* scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredScalarSurface* visData = nullptr;
	double* scalarRange = nullptr;

	void AssembleNode(osg::Node* parent);

	void AddNodeContour(osg::Node* parent);
	void AddNodePoints(osg::Node* parent);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetPointSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;

	std::list<vtkObject*> objectsToDelete;
};