// @otlicense
// File: CoordinateSystemManager.cpp
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

#include "CoordinateSystemManager.h"
#include "EdgesOperationBase.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCommunication/ActionTypes.h"

#include "OTModelEntities/EntityCoordinateSystem.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityAPI.h"

#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/EntityBrep.h"

#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Edge.hxx"
#include "NCollection_DataMap.hxx"
#include "TopTools_ShapeMapHasher.hxx"
		 
#include "TopoDS_Face.hxx"
		 
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
		 
#include "BRep_Tool.hxx"
#include "Geom_Surface.hxx"
#include "GeomAPI_ProjectPointOnSurf.hxx"
#include "GeomLProp_SLProps.hxx"

#include "BRepAdaptor_Curve.hxx"
#include "GCPnts_AbscissaPoint.hxx"
		 
#include "TopLoc_Location.hxx"
#include "TopAbs_Orientation.hxx"
		 
#include "gp_Pnt.hxx"
#include "gp_Dir.hxx"
#include "gp_Vec.hxx"
#include "gp_Trsf.hxx"
#include "gp.hxx"

void CoordinateSystemManager::createNew()
{
	std::string itemName = createUniqueName("Coordinate Systems/LCS");

	EntityCoordinateSystem* csEntity = new EntityCoordinateSystem(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	csEntity->setName(itemName);
	csEntity->createProperties();
	csEntity->setTreeItemEditable(true);
	csEntity->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		serviceName
	);

	csEntity->storeToDataBase();

	std::list<ot::UID> topologyEntityIDList = { csEntity->getEntityID()};
	std::list<ot::UID> topologyEntityVersionList = { csEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, {}, {}, {}, "create new local coordinate system: " + itemName);
}

void CoordinateSystemManager::activateCoordinateSystem(const std::string &csName)
{
	if (csName == activeCoordinateSystemName) return; // No change

	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation({ activeCoordinateSystemName, csName }, entityInfo);

	DataBase::instance().prefetchDocumentsFromStorage(entityInfo);

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible;

	setActiveFlagForCoordinateSystemEntity(entityInfo.front(), false, topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible);
	setActiveFlagForCoordinateSystemEntity(entityInfo.back(),  true,  topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible);

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, {}, {}, {}, "activate coordinate system: " + (csName.empty() ? "Global" : csName));

	activeCoordinateSystemName = csName;
}

void CoordinateSystemManager::setActiveCoordinateSystem(const std::string& csName)
{
	activeCoordinateSystemName = csName;
}

std::string CoordinateSystemManager::createUniqueName(const std::string& name)
{
	// Find the parent folder name of the entity
	std::string parentFolder;
	size_t index = name.rfind('/');

	if (index != std::string::npos)
	{
		parentFolder = name.substr(0, index);
	}

	// Get the list of items in the parent folder
	std::list<std::string> folderItems = ot::ModelServiceAPI::getListOfFolderItems(parentFolder);

	// Now make sure that the new name of the item is unique
	std::string itemName = name;
	if (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end())
	{
		int count = 1;
		do
		{
			itemName = name + "_" + std::to_string(count);
			count++;
		} while (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end());
	}

	return itemName;
}

void CoordinateSystemManager::setActiveFlagForCoordinateSystemEntity(const ot::EntityInformation &entityInfo, bool isActive, std::list<ot::UID>& topologyEntityIDList, std::list<ot::UID>& topologyEntityVersionList, std::list<bool>& topologyEntityForceVisible)
{
	EntityBase *entity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
	if (entity == nullptr) return;

	EntityCoordinateSystem* csEntity = dynamic_cast<EntityCoordinateSystem*>(entity);
	if (csEntity == nullptr) return;

	csEntity->setActive(isActive);
	csEntity->storeToDataBase();

	topologyEntityIDList.push_back(csEntity->getEntityID());
	topologyEntityVersionList.push_back(csEntity->getEntityStorageVersion());
	topologyEntityForceVisible.push_back(false);
}

void CoordinateSystemManager::alignFaceEdge()
{
	// Check whether the currently active coordinate system is the global one
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(activeCoordinateSystemName, entityInfo);

	if (activeCoordinateSystemName.empty() || isGlobalCoordinateSystem(entityInfo.getEntityID(), entityInfo.getEntityVersion()))
	{
		uiComponent->displayErrorPrompt("The currently active coordinate system is the global system and cannot be modified.");
		return;
	}

	// We first need to pick a face for the coordinate system
	std::map<std::string, std::string> options;
	uiComponent->enterEntitySelectionMode(ot::ModelServiceAPI::getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::FACE, false, "", ot::components::UiComponent::entitySelectionAction::ALIGN_LCS_FACE, "align the local coordinate system origin and normal direction", options, serviceID);
}

void CoordinateSystemManager::facePicked(const std::string& selectionInfo)
{
	// First we load the corresponding cad entity
	ot::JsonDocument doc;
	doc.fromJson(selectionInfo);

	ot::ConstJsonArray modelID = ot::json::getArray(doc, "modelID");
	ot::ConstJsonArray faceName = ot::json::getArray(doc, "faceName");

	if (modelID.Size() != 1 || faceName.Size() != 1)
	{
		assert(0); // This should not happen, since we always want to pick exactly one face from one object
		return;
	}

	ot::EntityInformation geometryInfo;
	ot::ModelServiceAPI::getEntityInformation({modelID[0].GetUint64()}, geometryInfo);

	std::unique_ptr<EntityGeometry> geometryEntity(dynamic_cast<EntityGeometry*> (ot::EntityAPI::readEntityFromEntityIDandVersion(geometryInfo.getEntityID(), geometryInfo.getEntityVersion())));
	if (geometryEntity == nullptr)
	{
		assert(0); // This should not happen, since we expect a geometry entity to be picked here
		return;
	}

	ot::EntityInformation brepInfo;
	ot::ModelServiceAPI::getEntityInformation({ (ot::UID) geometryEntity->getBrepStorageObjectID() }, brepInfo);

	std::unique_ptr<EntityBrep> brepEntity(dynamic_cast<EntityBrep*> (ot::EntityAPI::readEntityFromEntityIDandVersion(brepInfo.getEntityID(), brepInfo.getEntityVersion())));
	if (brepEntity == nullptr)
	{
		assert(0); // This should not happen, since we expect a brep entity to be available here
		return;
	}

	TopoDS_Shape face;
	if (!findFaceFromName(brepEntity.get(), faceName[0].GetString(), face))
	{
		assert(0); // This should not happen, since we just picked the face, so it should still be there
		return;
	}

	// Get the face center (-> origin) and the corresponding normal direction (-> z axis) from the face object
	if (!getFaceCentroidAndNormal(face, lcsOrigin, lcsNormal))
	{
		uiComponent->displayErrorPrompt("The origin and face normal cannot be determined from the selected face.");
		return;
	}

	// After successfully picking the face, we now need to pick the edge for the x-axis orientation
	std::map<std::string, std::string> options;
	uiComponent->enterEntitySelectionMode(ot::ModelServiceAPI::getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::EDGE, false, "", ot::components::UiComponent::entitySelectionAction::ALIGN_LCS_EDGE, "align the local coordinate system x-axis", options, serviceID);
}

void CoordinateSystemManager::edgePicked(const std::string& selectionInfo)
{
	ot::JsonDocument doc;
	doc.fromJson(selectionInfo);

	ot::ConstJsonArray modelID = ot::json::getArray(doc, "modelID");
	ot::ConstJsonArray edgeName = ot::json::getArray(doc, "edgeName");

	if (modelID.Size() != 1 || edgeName.Size() != 1)
	{
		assert(0); // This should not happen, since we always want to pick exactly one edge from one object
		return;
	}

	ot::EntityInformation geometryInfo;
	ot::ModelServiceAPI::getEntityInformation({ modelID[0].GetUint64() }, geometryInfo);

	std::unique_ptr<EntityGeometry> geometryEntity(dynamic_cast<EntityGeometry*> (ot::EntityAPI::readEntityFromEntityIDandVersion(geometryInfo.getEntityID(), geometryInfo.getEntityVersion())));
	if (geometryEntity == nullptr)
	{
		assert(0); // This should not happen, since we expect a geometry entity to be picked here
		return;
	}

	ot::EntityInformation brepInfo;
	ot::ModelServiceAPI::getEntityInformation({ (ot::UID)geometryEntity->getBrepStorageObjectID() }, brepInfo);

	std::unique_ptr<EntityBrep> brepEntity(dynamic_cast<EntityBrep*> (ot::EntityAPI::readEntityFromEntityIDandVersion(brepInfo.getEntityID(), brepInfo.getEntityVersion())));
	if (brepEntity == nullptr)
	{
		assert(0); // This should not happen, since we expect a brep entity to be available here
		return;
	}

	TopoDS_Shape edge;
	if (!findEdgeFromName(brepEntity.get(), edgeName[0].GetString(), edge))
	{
		assert(0); // This should not happen, since we just picked the face, so it should still be there
		return;
	}

	// Get the edge center and the corresponding edge direction (-> x-axis) from the edge object
	gp_Pnt edgeMidPoint;
	gp_Vec edgeMidDirection;

	if (!getEdgeMidpointAndDirection(edge, edgeMidPoint, edgeMidDirection))
	{
		uiComponent->displayErrorPrompt("The center edge direction cannot be determine from the selected edge.");
		return;
	}

	if (lcsNormal.Dot(gp_Dir(edgeMidDirection)) > cos(1e-3))
	{
		uiComponent->displayErrorPrompt("The selected edge direction is parallel to the selected face normal.");
		return;
	}

	// Finally, we assign the new data to the currently active coordinate system
	updateActiveLocalCoordinateSystem(lcsOrigin, lcsNormal, edgeMidDirection);
}

bool CoordinateSystemManager::isGlobalCoordinateSystem(ot::UID entityID, ot::UID entityVersion)
{
	EntityCoordinateSystem* csEntity = dynamic_cast<EntityCoordinateSystem*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion));

	if (csEntity != nullptr)
	{
		bool isGlobal = csEntity->getGlobal();

		delete csEntity;
		csEntity = nullptr;

		return isGlobal;
	}

	return false;
}

bool CoordinateSystemManager::findFaceFromName(EntityBrep* brepEntity, const std::string& faceName, TopoDS_Shape& face)
{
	TopExp_Explorer exp;
	for (exp.Init(brepEntity->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		if (brepEntity->getFaceName(aFace) == faceName)
		{
			face = exp.Current();
			return true;
		}
	}

	return false;
}

bool CoordinateSystemManager::findEdgeFromName(EntityBrep* brepEntity, const std::string& edgeName, TopoDS_Shape& edge)
{
	NCollection_DataMap<TopoDS_Edge, std::list<std::string>, TopTools_ShapeMapHasher> allEdgeNames;

	TopExp_Explorer exp;
	for (exp.Init(brepEntity->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		TopExp_Explorer expE;
		for (expE.Init(aFace, TopAbs_EDGE); expE.More(); expE.Next())
		{
			const TopoDS_Edge aEdge = TopoDS::Edge(expE.Current());

			const auto faceIt = brepEntity->getFaceNameMap().find(aFace.TShape());

			if (faceIt != brepEntity->getFaceNameMap().end())
			{
				const std::string& faceName = faceIt->second;

				if (allEdgeNames.IsBound(aEdge))
				{
					allEdgeNames.ChangeFind(aEdge).push_back(faceName);
				}
				else
				{
					std::list<std::string> names;
					names.push_back(faceName);
					allEdgeNames.Bind(aEdge, names);
				}
			}
		}
	}

	NCollection_DataMap<TopoDS_Edge, std::list<std::string>, TopTools_ShapeMapHasher>::Iterator it(allEdgeNames);

	for (; it.More(); it.Next())
	{
		const TopoDS_Edge& thisEdge = it.Key();
		const std::list<std::string>& names = it.Value();

		if (names.size() == 2)
		{
			const std::string& face1 = names.front();
			const std::string& face2 = names.back();

			std::string thisEdgeName = (face1 <= face2) ? face1 + ";" + face2 : face2 + ";" + face1;

			if (thisEdgeName == edgeName)
			{
				edge = thisEdge;
				return true;
			}
		}
	}

	return false;
}

bool CoordinateSystemManager::getFaceCentroidAndNormal(const TopoDS_Shape& faceShape, gp_Pnt& outCentroid, gp_Dir& outNormal)
{
	TopoDS_Face face = TopoDS::Face(faceShape);
	if (face.IsNull())
		return false;

	// Compute area properties of the trimmed face; CentreOfMass is in model space.
	GProp_GProps gprops;
	BRepGProp::SurfaceProperties(face, gprops);
	gp_Pnt centroidModel = gprops.CentreOfMass();

	// Get the underlying surface and its location (placement) from the face.
	TopLoc_Location loc;
	Handle(Geom_Surface) surf = BRep_Tool::Surface(face, loc);
	if (surf.IsNull())
		return false;

	// Convert centroid from model space to the surface's local space (inverse location),
	// because projection expects point coordinates in the surface's parametric space reference.
	gp_Trsf invLoc = loc.Transformation();
	invLoc.Invert();
	gp_Pnt centroidLocal = centroidModel;
	centroidLocal.Transform(invLoc);

	// Project the centroid onto the surface to obtain robust (u,v) parameters.
	GeomAPI_ProjectPointOnSurf proj(centroidLocal, surf);
	if (proj.NbPoints() < 1)
		return false;

	Standard_Real u = 0.0, v = 0.0;
	proj.LowerDistanceParameters(u, v);

	// Evaluate point on surface at (u,v), then transform back to model space.
	gp_Pnt pLocal = surf->Value(u, v);
	gp_Pnt pModel = pLocal;
	pModel.Transform(loc.Transformation());

	// Compute surface normal from first derivatives at (u,v).
	// The '1' means compute up to first derivatives; tolerance controls numerical stability.
	GeomLProp_SLProps slProps(surf, u, v, 1, 1e-9);
	if (!slProps.IsNormalDefined())
		return false;

	gp_Dir nLocal = slProps.Normal();
	gp_Dir nModel = nLocal;
	nModel.Transform(loc.Transformation());

	// Respect face orientation: REVERSED flips the geometric normal.
	if (face.Orientation() == TopAbs_REVERSED)
		nModel.Reverse();

	outCentroid = centroidModel; // actual area centroid of the trimmed face
	outNormal = nModel;          // normal evaluated at the projected centroid parameters
	return true;
}

bool CoordinateSystemManager::getEdgeMidpointAndDirection(const TopoDS_Shape& edgeShape, gp_Pnt& outMidPoint, gp_Vec& outDirection)
{
	TopoDS_Edge edge = TopoDS::Edge(edgeShape);
	if (edge.IsNull())
		return false;

	// BRepAdaptor_Curve automatically handles edge location and geometry
	BRepAdaptor_Curve curve(edge);

	const Standard_Real first = curve.FirstParameter();
	const Standard_Real last = curve.LastParameter();

	// Compute total arc length of the edge
	const Standard_Real length =
		GCPnts_AbscissaPoint::Length(curve, first, last);

	if (length <= gp::Resolution())
		return false; // Degenerated edge

	// Compute parameter at half arc length
	GCPnts_AbscissaPoint abscissa(curve, 0.5 * length, first);
	if (!abscissa.IsDone())
		return false;

	const Standard_Real uMid = abscissa.Parameter();

	// Evaluate point and first derivative (tangent)
	gp_Pnt P;
	gp_Vec V;
	curve.D1(uMid, P, V);

	if (V.Magnitude() <= gp::Resolution())
		return false;

	// Respect edge orientation: reversed edges invert tangent
	if (edge.Orientation() == TopAbs_REVERSED)
		V.Reverse();

	outMidPoint = P;
	outDirection = V.Normalized();

	return true;
}

void CoordinateSystemManager::updateActiveLocalCoordinateSystem(gp_Pnt lcsOrigin, gp_Dir lcsNormal, gp_Vec edgeMidDirection)
{
	ot::EntityInformation csEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(activeCoordinateSystemName, csEntityInfo);

	std::unique_ptr<EntityCoordinateSystem> csEntity(dynamic_cast<EntityCoordinateSystem*>(ot::EntityAPI::readEntityFromEntityIDandVersion(csEntityInfo.getEntityID(), csEntityInfo.getEntityVersion())));

	if (csEntity != nullptr)
	{
		// Set the coordinate system properties
		setValue(csEntity.get(), "Origin", "X", lcsOrigin.X());
		setValue(csEntity.get(), "Origin", "Y", lcsOrigin.Y());
		setValue(csEntity.get(), "Origin", "Z", lcsOrigin.Z());

		setValue(csEntity.get(), "x-Axis", "X", edgeMidDirection.X());
		setValue(csEntity.get(), "x-Axis", "Y", edgeMidDirection.Y());
		setValue(csEntity.get(), "x-Axis", "Z", edgeMidDirection.Z());

		setValue(csEntity.get(), "z-Axis", "X", lcsNormal.X());
		setValue(csEntity.get(), "z-Axis", "Y", lcsNormal.Y());
		setValue(csEntity.get(), "z-Axis", "Z", lcsNormal.Z());

		// Store the updated entity
		csEntity->storeToDataBase();

		std::list<ot::UID> topologyEntityIDList = { csEntity->getEntityID() };
		std::list<ot::UID> topologyEntityVersionList = { csEntity->getEntityStorageVersion() };
		std::list<bool> topologyEntityForceVisible = { false };

		ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, {}, {}, {}, "align local coordinate system: " + csEntity->getName());
	}

	uiComponent->refreshSelection(ot::ModelServiceAPI::getCurrentVisualizationModelID());
}

void CoordinateSystemManager::setValue(EntityCoordinateSystem* csEntity, const std::string& groupName, const std::string& propName, double value)
{
	EntityPropertiesDouble* valueProperty = dynamic_cast<EntityPropertiesDouble*>(csEntity->getProperties().getProperty("#" + propName, groupName));
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(csEntity->getProperties().getProperty(propName, groupName));
	assert(valueProperty != nullptr && stringProperty != nullptr);

	if (valueProperty != nullptr)
	{
		valueProperty->setValue(value);
	}

	if (stringProperty != nullptr)
	{
		std::ostringstream oss;
		oss << std::defaultfloat << value;

		stringProperty->setValue(oss.str());
	}
}
