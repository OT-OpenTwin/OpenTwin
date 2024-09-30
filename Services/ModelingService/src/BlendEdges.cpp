#include "BlendEdges.h"
#include "EntityGeometry.h"
#include "EntityFaceAnnotation.h"
#include "DataBase.h"
#include "EntityCache.h"
#include "UpdateManager.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

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
	return "BlendEdgesVisible";
}

std::string BlendEdges::getHiddenTreeItemName()
{
	return "BlendEdgesHidden";
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

	return (chamferWidth != 0.0);
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
