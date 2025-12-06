// @otlicense
// File: BlendEdges.cpp
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

#include "BlendEdges.h"
#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "DataBase.h"
#include "EntityCache.h"
#include "UpdateManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepAlgoAPI_Defeaturing.hxx"
#include <BRepFilletAPI_MakeFillet.hxx>

#include <map>
#include <set>

ot::components::UiComponent::entitySelectionAction BlendEdges::getSelectionAction()
{
	return ot::components::UiComponent::entitySelectionAction::BLEND_EDGE;
}

std::string BlendEdges::getOperationDescription()
{
	return "blend";
}

std::string BlendEdges::getVisibleTreeItemName()
{
	return "Default/BlendEdgesVisible";
}

std::string BlendEdges::getHiddenTreeItemName()
{
	return "Default/BlendEdgesHidden";
}

std::string BlendEdges::getShapeType()
{
	return "BLEND_EDGES";
}

void BlendEdges::addSpecificProperties(EntityGeometry* geometryEntity)
{
	addParametricProperty(geometryEntity, "Blend radius", 0.0);
}

bool BlendEdges::operationActive(EntityGeometry* geometryEntity)
{
	EntityPropertiesDouble* width = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Blend radius"));
	double chamferWidth = width->getValue();

	EntityPropertiesBoolean* active = dynamic_cast<EntityPropertiesBoolean*>(geometryEntity->getProperties().getProperty("Active"));
	bool operationActive = active->getValue();

	return (chamferWidth != 0.0 && operationActive);
}

bool BlendEdges::performActualOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, TopoDS_Shape& shape, TopTools_ListOfShape& listOfProcessedEdges, BRepTools_History*& history)
{
	listOfProcessedEdges.Clear();
	history = nullptr;

	// Perform the fillet operation
	BRepFilletAPI_MakeFillet MF(baseBrep->getBrep());
	TopExp_Explorer exp;

	EntityPropertiesDouble* width = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Blend radius"));
	double chamferWidth = width->getValue();

	for (exp.Init(baseBrep->getBrep(), TopAbs_EDGE); exp.More(); exp.Next())
	{
		TopoDS_Edge edge = TopoDS::Edge(exp.Current());

		if (allEdgesForOperation.count(edge.TShape()) != 0)
		{
			MF.Add(chamferWidth, TopoDS::Edge(exp.Current()));
			listOfProcessedEdges.Append(exp.Current());
		}
	}

	try
	{
		shape = MF.Shape();
	}
	catch (Standard_Failure)
	{
		return false;
	}

	TopTools_ListOfShape anArguments;
	anArguments.Append(baseBrep->getBrep());

	history = new BRepTools_History(anArguments, MF);

	return true;
}
