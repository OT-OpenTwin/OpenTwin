// @otlicense
// File: VtkDriverCartesianVectorfield.h
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
#include "PropertyBundleDataHandleVis2D3D.h"
#include "DataSourceResult3D.h"

namespace osg {
	class Node;
}

class VtkDriverCartesianVectorfield : public VtkDriver {
public:
	VtkDriverCartesianVectorfield();
	virtual ~VtkDriverCartesianVectorfield();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;
	
private:
	PropertyBundleDataHandlePlane * planeData = nullptr;
	PropertyBundleDataHandleScaling * scalingData = nullptr;
	PropertyBundleDataHandleVis2D3D * vis2D3DData = nullptr;
	double * scalarRange = nullptr;

	vtkAlgorithmOutput* ApplyCutplane(DataSourceResult3D *source, osg::Node *parent);
	void AssembleNode (vtkAlgorithmOutput * input, osg::Node *parent);
	vtkAlgorithmOutput* AddNodeVectors(vtkAlgorithmOutput* input);
	vtkAlgorithmOutput* SetScalarValues(vtkAlgorithmOutput* input);
	void SetColouring(vtkPolyDataMapper* mapper);

	void CheckForModelUpdates();
	void DeletePropertyData(void);
};