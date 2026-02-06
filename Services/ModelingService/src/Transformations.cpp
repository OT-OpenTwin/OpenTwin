// @otlicense
// File: Transformations.cpp
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

#include "Transformations.h"

#include "OTCADEntities/EntityGeometry.h"
#include "EntityCache.h"
#include "UpdateManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/EntityInformation.h"
#include "OTModelEntities/EntityCoordinateSystem.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/DataBase.h"

#include <string>
#include <list>
#include <map>

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>

void Transformations::enterTransformMode(const std::list<ot::EntityInformation> &selectedGeometryEntities, const std::list<ot::EntityInformation>& selectedCoordinateSystemEntities)
{
	if (selectedGeometryEntities.size() > 0 && selectedCoordinateSystemEntities.size() == 0)
	{
		std::map<std::string, std::string> options;
		int count = 1;
		for (auto entity : selectedGeometryEntities)
		{
			options[std::to_string(count)] = entity.getEntityName();
			count++;
		}

		uiComponent->enterEntitySelectionMode(ot::ModelServiceAPI::getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::TRANSFORM,
			false, "", ot::components::UiComponent::entitySelectionAction::TRANSFORM_SHAPES, "transform", options, serviceID);
	}
	else if (selectedGeometryEntities.size() == 0 && selectedCoordinateSystemEntities.size() == 1)
	{
		if (isGlobalCoordinateSystem(selectedCoordinateSystemEntities.front().getEntityID(), selectedCoordinateSystemEntities.front().getEntityVersion()))
		{
			uiComponent->displayErrorPrompt("The selected coordinate system is the global system and cannot be transformed.");
			return;
		}

		std::map<std::string, std::string> options;
		options["1"] = selectedCoordinateSystemEntities.front().getEntityName();

		uiComponent->enterEntitySelectionMode(ot::ModelServiceAPI::getCurrentVisualizationModelID(), ot::components::UiComponent::entitySelectionType::TRANSFORM,
			false, "", ot::components::UiComponent::entitySelectionAction::TRANSFORM_LOCALCOORDINATESYSTEM, "transform", options, serviceID);
	}
	else
	{
		uiComponent->displayErrorPrompt("Please select either one or more geometry objects or a single local coordinate system before entering the transform mode");
		return;
	}
}

gp_Trsf Transformations::setTransform(EntityGeometry *geomEntity, TopoDS_Shape &shape, gp_Trsf prevTransform)
{
	EntityPropertiesDouble *xposProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Position X"));
	EntityPropertiesDouble *yposProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Position Y"));
	EntityPropertiesDouble *zposProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Position Z"));
	EntityPropertiesDouble *xAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Axis X"));
	EntityPropertiesDouble *yAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Axis Y"));
	EntityPropertiesDouble *zAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Axis Z"));
	EntityPropertiesDouble *angleProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Angle (deg.)"));

	if (xposProperty == nullptr || yposProperty == nullptr || zposProperty == nullptr || xAxisProperty == nullptr || yAxisProperty == nullptr || zAxisProperty == nullptr || angleProperty == nullptr)
	{
		assert(0); // Some properties are missing
		return prevTransform;
	}

	double xp = xposProperty->getValue();
	double yp = yposProperty->getValue();
	double zp = zposProperty->getValue();

	double vx = xAxisProperty->getValue();
	double vy = yAxisProperty->getValue();
	double vz = zAxisProperty->getValue();

	double angle = angleProperty->getValue() / 180.0 * M_PI;

	gp_Trsf transformR, transformT;

	if (vx != 0.0 || vy != 0.0 || vz != 0.0)
	{
		transformR.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(vx, vy, vz)), angle);
	}

	transformT.SetTranslation(gp_Vec(xp, yp, zp));

	gp_Trsf transform = transformT * transformR;

	/*
	gp_XYZ oldAxis;
	double oldAngle = 0.0;
	if (prevTransform.GetRotation(oldAxis, oldAngle))
	{
	std::cout << "rotation\n";
	}

	gp_XYZ oldTranslation = prevTransform.TranslationPart();
	*/

	gp_Trsf undoTransform  = prevTransform.Inverted();
	gp_Trsf totalTransform = transform * undoTransform;

	BRepBuilderAPI_Transform shapeTransform(totalTransform);
	shapeTransform.Perform(shape, false);

	shape = shapeTransform.Shape();

	/*
	BRepBuilderAPI_Transform shapeTransformUndo(undoTransform);
	shapeTransformUndo.Perform(shape, false);

	shape = shapeTransformUndo.Shape();

	bool applyTransform = true;
	if (applyTransform)
	{ 
	BRepBuilderAPI_Transform shapeTransform(transform);
	shapeTransform.Perform(shape, false);

	shape = shapeTransform.Shape();
	}
	*/

	return transform;
}

void Transformations::transformShapes(const std::string &selectionInfo, std::map<std::string, std::string> &options)
{
	// Lock the user interface
	ot::LockTypes lockFlags(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);

	uiComponent->lockUI(lockFlags);

	// Determine the information about all shapes to be transformed
	std::list<std::string> shapeNames;

	for (auto item : options)
	{
		std::string name = item.second;
		shapeNames.push_back(name);
	}

	std::list<ot::EntityInformation> geometryEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(shapeNames, geometryEntityInfo);

	// Now prefetch all geometry entities (except for the ones which are already in the cache)
	entityCache->prefetchEntities(geometryEntityInfo);

	// In a next step, we determine a list of all breps which need to be loaded later as well
	std::list<ot::UID> requiredBreps;
	std::list<EntityGeometry *> geometryEntities;

	for (auto entity : geometryEntityInfo)
	{
		EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entity.getEntityID(), entity.getEntityVersion()));

		if (geometryEntity != nullptr)
		{
			geometryEntities.push_back(geometryEntity);
			requiredBreps.push_back(geometryEntity->getBrepStorageObjectID());
		}
	}

	// Now get the information about all the breps
	std::list<ot::EntityInformation> brepEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(requiredBreps, brepEntityInfo);

	// Prefetch all the brep entities
	entityCache->prefetchEntities(brepEntityInfo);

	// Now we go through all geometry entities and update their transformation information

	ot::ModelServiceAPI::enableMessageQueueing(true);

	// Read the transformation properties
	rapidjson::Document doc;
	doc.Parse(selectionInfo.c_str());

	gp_XYZ transformTranslate;
	transformTranslate.SetX(doc["Translate X"].GetDouble());
	transformTranslate.SetY(doc["Translate Y"].GetDouble());
	transformTranslate.SetZ(doc["Translate Z"].GetDouble());

	gp_XYZ transformAxis;
	transformAxis.SetX(doc["Axis X"].GetDouble());
	transformAxis.SetY(doc["Axis Y"].GetDouble());
	transformAxis.SetZ(doc["Axis Z"].GetDouble());

	double transformAngle = doc["Angle"].GetDouble() / 180.0 * M_PI;

	bool hasRotation = transformAngle != 0.0 && (transformAxis.X() != 0.0 || transformAxis.Y() != 0.0 || transformAxis.Z() != 0.0);

	gp_XYZ rotationCenter;

	if (hasRotation)
	{
		// Determine the exact bounding box
		Bnd_Box boundingBox;

		for (auto brep : brepEntityInfo)
		{
			// Now we load the corresponding brep 
			EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brep.getEntityID(), brep.getEntityVersion()));
			if (brepEntity == nullptr) continue;

			BRepBndLib tightBox;
			tightBox.AddOptimal(brepEntity->getBrep(), boundingBox);
		}

		gp_Pnt cmin = boundingBox.CornerMin();
		gp_Pnt cmax = boundingBox.CornerMax();

		rotationCenter = gp_XYZ(0.5*(cmin.X() + cmax.X()), 0.5*(cmin.Y() + cmax.Y()), 0.5*(cmin.Z() + cmax.Z()));
	}
	else
	{
		transformAngle = 0.0;
	}

	// Now we apply the actual transformations
	std::list<ot::UID> modifiedEntities;
	auto brep = brepEntityInfo.begin();

	for (auto geometryEntity : geometryEntities)
	{
		// update the transformation properties
		updateTransformationProperties(geometryEntity, transformTranslate, transformAxis, transformAngle, rotationCenter);

		// Now we load the corresponding brep (since this was already loaded for the bounding box calc, it will come from the cache)
		EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brep->getEntityID(), brep->getEntityVersion()));
		if (brepEntity == nullptr)
		{
			brep++;
			continue;
		}

		TopoDS_Shape shape = brepEntity->getBrep();

		// Transform the brep and create a new brep entity
		gp_Trsf newTransform = setTransform(geometryEntity, shape, brepEntity->getTransform());

		// Store the new brep and facet entity (create new entity IDs)
		geometryEntity->resetBrep();
		geometryEntity->resetFacets();

		geometryEntity->getBrepEntity()->copyFaceNames(brepEntity);
		geometryEntity->getBrepEntity()->setEntityID(modelComponent->createEntityUID());
		geometryEntity->getFacets()->setEntityID(modelComponent->createEntityUID());

		// Add the new brep entity to the geometry entity
		geometryEntity->setBrep(shape);
		geometryEntity->getBrepEntity()->setTransform(newTransform);

		// Facet the geometry entity and store it
		geometryEntity->facetEntity(false);

		geometryEntity->storeBrep();

		ot::UID brepStorageVersion = geometryEntity->getBrepEntity()->getEntityStorageVersion();

		entityCache->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		geometryEntity->storeFacets();

		ot::UID facetsStorageVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		geometryEntity->releaseFacets();

		// Add the entitiy to the list of modified entities
		modifiedEntities.push_back(geometryEntity->getEntityID());

		// Store the information about the entities such that they can be added to the model

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_UpdateGeometryEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, geometryEntity->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Brep, (unsigned long long) geometryEntity->getBrepStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID_Facets, (unsigned long long) geometryEntity->getFacetsStorageObjectID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_OverrideGeometry, false, doc.GetAllocator());	// The modified entity was not yet written 

		ot::PropertyGridCfg cfg;
		geometryEntity->getProperties().addToConfiguration(nullptr, false, cfg);

		ot::JsonObject cfgObj;
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_MODEL_NewProperties, cfgObj, doc.GetAllocator());
		
		std::string tmp;
		modelComponent->sendMessage(false, doc, tmp);

		delete geometryEntity;
		geometryEntity = nullptr;

		// Switch to the next brep entity
		brep++;
	}

	// Now we ask the model service to check for the potential updates of the parent entities
	getUpdateManager()->checkParentUpdates(modifiedEntities);

	// Finally, store the new model state
	ot::ModelServiceAPI::modelChangeOperationCompleted("transform shapes");
	ot::ModelServiceAPI::enableMessageQueueing(false);
	uiComponent->refreshSelection(ot::ModelServiceAPI::getCurrentVisualizationModelID());

	// Unlock the ui
	uiComponent->unlockUI(lockFlags);
}

void Transformations::transformCoordinateSystem(const std::string& selectionInfo, std::map<std::string, std::string>& options)
{
	// Lock the user interface
	ot::LockTypes lockFlags(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);

	uiComponent->lockUI(lockFlags);

	// Determine the information about all coordinate system to be transformed
	std::list<std::string> csNames;

	for (auto item : options)
	{
		std::string name = item.second;
		csNames.push_back(name);
	}

	std::list<ot::EntityInformation> csEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(csNames, csEntityInfo);

	// Now prefetch all geometry entities (except for the ones which are already in the cache)
	DataBase::instance().prefetchDocumentsFromStorage(csEntityInfo);

	// Read the transformation properties
	rapidjson::Document doc;
	doc.Parse(selectionInfo.c_str());

	gp_XYZ transformTranslate;
	transformTranslate.SetX(doc["Translate X"].GetDouble());
	transformTranslate.SetY(doc["Translate Y"].GetDouble());
	transformTranslate.SetZ(doc["Translate Z"].GetDouble());

	gp_XYZ transformAxis;
	transformAxis.SetX(doc["Axis X"].GetDouble());
	transformAxis.SetY(doc["Axis Y"].GetDouble());
	transformAxis.SetZ(doc["Axis Z"].GetDouble());

	double transformAngle = doc["Angle"].GetDouble() / 180.0 * M_PI;

	bool hasRotation = transformAngle != 0.0 && (transformAxis.X() != 0.0 || transformAxis.Y() != 0.0 || transformAxis.Z() != 0.0);
	if (!hasRotation) transformAngle = 0.0;

	gp_XYZ rotationCenter(0.0, 0.0, 0.0);

	EntityCoordinateSystem*csEntity = dynamic_cast<EntityCoordinateSystem*>(ot::EntityAPI::readEntityFromEntityIDandVersion(csEntityInfo.front().getEntityID(), csEntityInfo.front().getEntityVersion()));

	if (csEntity != nullptr)
	{
		// Now update the transformation
		updateTransformationProperties(csEntity, transformTranslate, transformAxis, transformAngle, rotationCenter);

		// Store the updated entity
		csEntity->storeToDataBase();

		std::list<ot::UID> topologyEntityIDList = { csEntity->getEntityID() };
		std::list<ot::UID> topologyEntityVersionList = { csEntity->getEntityStorageVersion() };
		std::list<bool> topologyEntityForceVisible = { false };

		ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, {}, {}, {}, "transform local coordinate system: " + csEntity->getName());

		delete csEntity;
	}

	uiComponent->refreshSelection(ot::ModelServiceAPI::getCurrentVisualizationModelID());

	// Unlock the ui
	uiComponent->unlockUI(lockFlags);
}


void Transformations::updateTransformationProperties(EntityGeometry *geometryEntity, gp_XYZ transformTranslate, gp_XYZ transformAxis, double transformAngle, gp_XYZ rotationCenter)
{
	EntityPropertiesDouble *xposProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Position X"));
	EntityPropertiesDouble *yposProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Position Y"));
	EntityPropertiesDouble *zposProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Position Z"));
	EntityPropertiesDouble *xAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Axis X"));
	EntityPropertiesDouble *yAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Axis Y"));
	EntityPropertiesDouble *zAxisProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Axis Z"));
	EntityPropertiesDouble *angleProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("#Angle (deg.)"));

	EntityPropertiesString *xposPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Position X"));
	EntityPropertiesString *yposPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Position Y"));
	EntityPropertiesString *zposPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Position Z"));
	EntityPropertiesString *xAxisPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Axis X"));
	EntityPropertiesString *yAxisPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Axis Y"));
	EntityPropertiesString *zAxisPropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Axis Z"));
	EntityPropertiesString *anglePropertyString = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("Angle (deg.)"));

	if (   xposProperty == nullptr || yposProperty == nullptr || zposProperty == nullptr || xAxisProperty == nullptr || yAxisProperty == nullptr || zAxisProperty == nullptr || angleProperty == nullptr
		|| xposPropertyString == nullptr || yposPropertyString == nullptr || zposPropertyString == nullptr || xAxisPropertyString == nullptr || yAxisPropertyString == nullptr || zAxisPropertyString == nullptr || anglePropertyString == nullptr) 
	{
		assert(0); // Some properties are missing
		return;
	}

	// We first get the current transform
	double xp = xposProperty->getValue();
	double yp = yposProperty->getValue();
	double zp = zposProperty->getValue();

	double vx = xAxisProperty->getValue();
	double vy = yAxisProperty->getValue();
	double vz = zAxisProperty->getValue();

	double angle = angleProperty->getValue() / 180.0 * M_PI;

	gp_Trsf transformR, transformT;

	if (vx != 0.0 || vy != 0.0 || vz != 0.0)
	{
		transformR.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(vx, vy, vz)), angle);
	}

	transformT.SetTranslation(gp_Vec(xp, yp, zp));

	gp_Trsf transform = transformT * transformR;

	// Now we determine the additional transformation
	gp_Trsf newTransform;

	// Handle the translation first
	gp_Vec transformTranslateVec(transformTranslate);
	transformTranslateVec.Transform(transform);

	gp_Trsf newTransformT;
	newTransformT.SetTranslation(transformTranslateVec);

	// Now consider the rotation
	if (transformAngle != 0.0)
	{
		gp_Trsf newTransformToCenter, newTransformFromCenter, newTransformCenterR, newTransformR;

		gp_Dir rotationAxis(transformAxis);
		rotationAxis.Transform(transform);

		newTransformToCenter.SetTranslation(-gp_Vec(rotationCenter));
		newTransformFromCenter.SetTranslation(gp_Vec(rotationCenter));
		newTransformCenterR.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), rotationAxis), transformAngle);
		newTransformR = newTransformFromCenter * newTransformCenterR * newTransformToCenter;

		newTransform = newTransformT * newTransformR;
	}
	else
	{
		newTransform = newTransformT;
	}

	// Compute the total transformation and set the properties
	transform = newTransform * transform;

	gp_XYZ translation = transform.TranslationPart();

	xposProperty->setValue(translation.X());
	yposProperty->setValue(translation.Y());
	zposProperty->setValue(translation.Z());

	xposPropertyString->setValue(to_string(translation.X()));
	yposPropertyString->setValue(to_string(translation.Y()));
	zposPropertyString->setValue(to_string(translation.Z()));

	gp_XYZ newAxis;
	double newAngle = 0.0;
	transform.GetRotation(newAxis, newAngle);
	newAngle *= 180.0 / M_PI;

	xAxisProperty->setValue(newAxis.X());
	yAxisProperty->setValue(newAxis.Y());
	zAxisProperty->setValue(newAxis.Z());
	angleProperty->setValue(newAngle);

	xAxisPropertyString->setValue(to_string(newAxis.X()));
	yAxisPropertyString->setValue(to_string(newAxis.Y()));
	zAxisPropertyString->setValue(to_string(newAxis.Z()));
	anglePropertyString->setValue(to_string(newAngle));
}

void Transformations::updateTransformationProperties(EntityCoordinateSystem* csEntity, gp_XYZ transformTranslate, gp_XYZ transformAxis, double transformAngle, gp_XYZ rotationCenter)
{
	// We first get the current transform
	gp_Pnt center(getValue(csEntity, "Origin", "X"), getValue(csEntity, "Origin", "Y"), getValue(csEntity, "Origin", "Z"));
	gp_Dir xDir  (getValue(csEntity, "x-Axis", "X"), getValue(csEntity, "x-Axis", "Y"), getValue(csEntity, "x-Axis", "Z"));
	gp_Dir zDir  (getValue(csEntity, "z-Axis", "X"), getValue(csEntity, "z-Axis", "Y"), getValue(csEntity, "z-Axis", "Z"));

	gp_Trsf transform = makeTrsfFromCenterXZ(center, xDir, zDir);
	transform.Invert();
	rotationCenter = gp_Pnt(rotationCenter).Transformed(transform).Coord();

	// Now we determine the additional transformation
	gp_Trsf newTransform;

	// Handle the translation first
	gp_Vec transformTranslateVec(transformTranslate);
	transformTranslateVec.Transform(transform);

	gp_Trsf newTransformT;
	newTransformT.SetTranslation(transformTranslateVec);

	// Now consider the rotation
	if (transformAngle != 0.0)
	{
		gp_Trsf newTransformToCenter, newTransformFromCenter, newTransformCenterR, newTransformR;

		gp_Dir rotationAxis(transformAxis);
		rotationAxis.Transform(transform);

		newTransformToCenter.SetTranslation(-gp_Vec(rotationCenter));
		newTransformFromCenter.SetTranslation(gp_Vec(rotationCenter));
		newTransformCenterR.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), rotationAxis), transformAngle);
		newTransformR = newTransformFromCenter * newTransformCenterR * newTransformToCenter;

		newTransform = newTransformT * newTransformR;
	}
	else
	{
		newTransform = newTransformT;
	}

	// Compute the total transformation and set the properties
	transform = newTransform * transform;// *newTransform;

	gp_XYZ origin = transform.TranslationPart();

	setValue(csEntity, "Origin", "X", origin.X());
	setValue(csEntity, "Origin", "Y", origin.Y());
	setValue(csEntity, "Origin", "Z", origin.Z());

	// The 3x3 linear (rotation) part of the transformation.
	const gp_Mat R = transform.VectorialPart();

	// For a local -> world transformation, the columns of R represent
	// the world-space directions of the local basis vectors.
	//
	// Column 1 -> local X-axis
	// Column 3 -> local Z-axis
	gp_Vec xV(R.Value(1, 1), R.Value(2, 1), R.Value(3, 1));
	gp_Vec zV(R.Value(1, 3), R.Value(2, 3), R.Value(3, 3));

	// Normalize defensively (in case of small numerical drift).
	if (xV.SquareMagnitude() > 0.0) xV.Normalize();
	if (zV.SquareMagnitude() > 0.0) zV.Normalize();

	setValue(csEntity, "x-Axis", "X", xV.X());
	setValue(csEntity, "x-Axis", "Y", xV.Y());
	setValue(csEntity, "x-Axis", "Z", xV.Z());	
	
	setValue(csEntity, "z-Axis", "X", zV.X());
	setValue(csEntity, "z-Axis", "Y", zV.Y());
	setValue(csEntity, "z-Axis", "Z", zV.Z());
}

// Center (origin) + X and Z axis directions -> gp_Trsf
// Result: transformation from the local standard coordinate system to the world frame
// defined by (center, xDir, zDir). Returns Identity in degenerate cases.
gp_Trsf Transformations::makeTrsfFromCenterXZ(const gp_Pnt& center, const gp_Dir& xDir_in, const gp_Dir& zDir_in)
{
	gp_Trsf identity; // Default-constructed gp_Trsf is Identity.

	// Convert directions to vectors for algebra.
	gp_Vec xV(xDir_in);
	gp_Vec zV(zDir_in);

	// If X and Z are (nearly) parallel, we cannot build a valid frame.
	// Since gp_Dir is normalized, dot is cos(angle).
	const Standard_Real dot = xV.Dot(zV);
	if (AbsR(AbsR(dot) - 1.0) < 1e-9) {
		return identity;
	}

	// Orthonormalize Z with respect to X (Gram-Schmidt):
	// z := z - (z·x) * x
	zV = zV - dot * xV;

	// If Z collapses numerically after orthogonalization, bail out.
	const Standard_Real tol2 = 1e-24; // squared tolerance
	if (zV.SquareMagnitude() < tol2) {
		return identity;
	}
	zV.Normalize();

	// Build a right-handed frame:
	// y = z × x  (so that x × y = z)
	gp_Vec yV = zV.Crossed(xV);
	if (yV.SquareMagnitude() < tol2) {
		return identity;
	}
	yV.Normalize();

	// Re-create normalized directions.
	gp_Dir xDir(xV);
	gp_Dir zDir(zV);

	// Define the target/world frame:
	// gp_Ax3 takes (location, mainDirection=Z, xDirection=X).
	gp_Ax3 worldFrame(center, zDir, xDir);

	// Define the source/local frame as the OCC standard XOY coordinate system.
	// (X along +X, Y along +Y, Z along +Z)
	gp_Ax3 localFrame(gp::XOY());

	// Build the transformation mapping local -> world.
	gp_Trsf trsf;
	trsf.SetTransformation(localFrame, worldFrame);
	return trsf;
}

double Transformations::getValue(EntityCoordinateSystem* csEntity, const std::string& groupName, const std::string& propName)
{
	EntityPropertiesDouble* property = dynamic_cast<EntityPropertiesDouble*>(csEntity->getProperties().getProperty("#" + propName, groupName));
	assert(property != nullptr);

	if (property == nullptr) return 0.0;

	return property->getValue();
}

void Transformations::setValue(EntityCoordinateSystem* csEntity, const std::string& groupName, const std::string& propName, double value)
{
	EntityPropertiesDouble* valueProperty  = dynamic_cast<EntityPropertiesDouble*>(csEntity->getProperties().getProperty("#" + propName, groupName));
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(csEntity->getProperties().getProperty(propName, groupName));
	assert(valueProperty != nullptr && stringProperty != nullptr);

	if (valueProperty != nullptr)
	{
		valueProperty->setValue(value);
	}

	if (stringProperty != nullptr)
	{
		stringProperty->setValue(toString(value));
	}
}

std::string Transformations::toString(double x)
{
	std::ostringstream oss;
	oss << std::defaultfloat << x;
	return oss.str();
}

bool Transformations::isGlobalCoordinateSystem(ot::UID entityID, ot::UID entityVersion)
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