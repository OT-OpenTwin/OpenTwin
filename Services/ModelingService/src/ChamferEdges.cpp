// @otlicense

#include "ChamferEdges.h"
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
#include <BRepFilletAPI_MakeChamfer.hxx>

#include <map>
#include <set>

ot::components::UiComponent::entitySelectionAction ChamferEdges::getSelectionAction()
{
	return ot::components::UiComponent::entitySelectionAction::CHAMFER_EDGE;
}

std::string ChamferEdges::getOperationDescription()
{
	return "chamfer";
}

std::string ChamferEdges::getVisibleTreeItemName()
{
	return "ChamferEdgesVisible";
}

std::string ChamferEdges::getHiddenTreeItemName()
{
	return "ChamferEdgesHidden";
}

std::string ChamferEdges::getShapeType()
{
	return "CHAMFER_EDGES";
}

void ChamferEdges::addSpecificProperties(EntityGeometry* geometryEntity)
{
	addParametricProperty(geometryEntity, "Chamfer width", 0.0);
}

bool ChamferEdges::operationActive(EntityGeometry* geometryEntity)
{
	EntityPropertiesDouble* width = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Chamfer width"));
	double chamferWidth = width->getValue();

	return (chamferWidth != 0.0);
}

bool ChamferEdges::performActualOperation(EntityGeometry* geometryEntity, EntityBrep* baseBrep, std::map< const opencascade::handle<TopoDS_TShape>, std::string>& allEdgesForOperation, TopoDS_Shape& shape, TopTools_ListOfShape& listOfProcessedEdges, BRepTools_History*& history)
{
	listOfProcessedEdges.Clear();
	history = nullptr;

	// Perform the fillet operation
	BRepFilletAPI_MakeChamfer MC(baseBrep->getBrep());
	TopExp_Explorer exp;

	EntityPropertiesDouble* width = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Chamfer width"));
	double chamferWidth = width->getValue();

	for (exp.Init(baseBrep->getBrep(), TopAbs_EDGE); exp.More(); exp.Next())
	{
		TopoDS_Edge edge = TopoDS::Edge(exp.Current());

		if (allEdgesForOperation.count(edge.TShape()) != 0)
		{
			MC.Add(chamferWidth, TopoDS::Edge(exp.Current()));
			listOfProcessedEdges.Append(exp.Current());
		}
	}

	try
	{
		shape = MC.Shape();
	}
	catch (Standard_Failure)
	{
		return false;
	}

	TopTools_ListOfShape anArguments;
	anArguments.Append(baseBrep->getBrep());

	history = new BRepTools_History(anArguments, MC);

	return true;
}
