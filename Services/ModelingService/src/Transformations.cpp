#include "Transformations.h"

#include "EntityGeometry.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"
#include "EntityCache.h"
#include "UpdateManager.h"

#include <OpenTwinCommunication/ActionTypes.h>
#include <OpenTwinFoundation/EntityInformation.h>
#include <OpenTwinFoundation/ModelComponent.h>
#include <OpenTwinFoundation/UiComponent.h>

#include <string>
#include <list>
#include <map>

#include <gp_Ax1.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>

void Transformations::enterTransformMode(const std::list<ot::EntityInformation> &selectedGeometryEntities)
{
	// First, we check which objects are currently selected

	if (selectedGeometryEntities.size() == 0)
	{
		uiComponent->displayErrorPrompt("Please select one or more geometry objects before entering the transform mode");
		return;
	}

	std::map<std::string, std::string> options;
	int count = 1;
	for (auto entity : selectedGeometryEntities)
	{
		options[std::to_string(count)] = entity.getName();
		count++;
	}

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySlectionType::TRANSFORM, 
		false, "", ot::components::UiComponent::entitySelectionAction::TRANSFORM_SHAPES, "transform", options, serviceID);
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

void Transformations::transformEntities(const std::string &selectionInfo, std::map<std::string, std::string> &options)
{
	// Lock the user interface
	ot::Flags<ot::ui::lockType> lockFlags;
	lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
	lockFlags.setFlag(ot::ui::lockType::tlNavigationWrite);
	lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
	lockFlags.setFlag(ot::ui::lockType::tlProperties);

	uiComponent->lockUI(lockFlags);

	// Determine the information about all shapes to be transformed
	std::list<std::string> shapeNames;

	for (auto item : options)
	{
		std::string name = item.second;
		shapeNames.push_back(name);
	}

	std::list<ot::EntityInformation> geometryEntityInfo;
	modelComponent->getEntityInformation(shapeNames, geometryEntityInfo);

	// Now prefetch all geometry entities (except for the ones which are already in the cache)
	entityCache->prefetchEntities(geometryEntityInfo);

	// In a next step, we determine a list of all breps which need to be loaded later as well
	std::list<ot::UID> requiredBreps;
	std::list<EntityGeometry *> geometryEntities;

	for (auto entity : geometryEntityInfo)
	{
		ClassFactoryCAD classFactory;
		ClassFactory baseFactory;
		classFactory.SetNextHandler(&baseFactory);
		baseFactory.SetChainRoot(&classFactory);

		EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry*>(modelComponent->readEntityFromEntityIDandVersion(entity.getID(), entity.getVersion(), classFactory));

		if (geometryEntity != nullptr)
		{
			geometryEntities.push_back(geometryEntity);
			requiredBreps.push_back(geometryEntity->getBrepStorageObjectID());
		}
	}

	// Now get the information about all the breps
	std::list<ot::EntityInformation> brepEntityInfo;
	modelComponent->getEntityInformation(requiredBreps, brepEntityInfo);

	// Prefetch all the brep entities
	entityCache->prefetchEntities(brepEntityInfo);

	// Now we go through all geometry entities and update their transformation information

	modelComponent->enableMessageQueueing(true);

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
			EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brep.getID(), brep.getVersion()));
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
		EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(brep->getID(), brep->getVersion()));
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

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateGeometryEntity);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, geometryEntity->getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID_Brep, (unsigned long long) geometryEntity->getBrepStorageObjectID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID_Facets, (unsigned long long) geometryEntity->getFacetsStorageObjectID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion_Brep, brepStorageVersion);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion_Facets, facetsStorageVersion);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_OverrideGeometry, false);	// The modified entity was not yet written 
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_NewProperties, geometryEntity->getProperties().getJSON(nullptr, false));	

		modelComponent->sendMessage(false, doc);

		delete geometryEntity;
		geometryEntity = nullptr;

		// Switch to the next brep entity
		brep++;
	}

	// Now we ask the model service to check for the potential updates of the parent entities
	getUpdateManager()->checkParentUpdates(modifiedEntities);

	// Finally, store the new model state
	modelComponent->modelChangeOperationCompleted("transform shapes");
	modelComponent->enableMessageQueueing(false);
	uiComponent->refreshSelection(modelComponent->getCurrentVisualizationModelID());

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
