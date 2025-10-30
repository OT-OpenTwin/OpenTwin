// @otlicense
// File: BlendEdges.h
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

#include "EdgesOperationBase.h"

#include "OldTreeIcon.h"
#include "ShapesBase.h"

#include "TopoDS_TShape.hxx"
#include "TopTools_ListOfShape.hxx"

class Model;
class EntityGeometry;
class EntityBrep;
class UpdateManager;

class TopoDS_Shape;
class BRepTools_History;

class BlendEdges : public EdgesOperationBase
{
public:
	BlendEdges(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache) 
		: EdgesOperationBase(_uiComponent, _modelComponent, _serviceID, _serviceName, _entityCache) {};
	virtual ~BlendEdges() {}

protected:
	void addSpecificProperties(EntityGeometry* geometryEntity);
	ot::components::UiComponent::entitySelectionAction getSelectionAction();
	std::string getOperationDescription();
	std::string getVisibleTreeItemName();
	std::string getHiddenTreeItemName();
	std::string getShapeType();
	bool operationActive(EntityGeometry* geometryEntity);
	bool performActualOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, TopoDS_Shape& shape, TopTools_ListOfShape &listOfProcessedEdges, BRepTools_History*& history);
};
