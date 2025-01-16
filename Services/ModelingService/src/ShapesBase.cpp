#include "ShapesBase.h"

#include "ApplicationSettings.h"
#include "EntityCache.h"
#include "DataBase.h"

#include "OTCore/CoreTypes.h"
#include "EntityGeometry.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "EntityInformation.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <STEPControl_Writer.hxx>
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"

#include <sstream>
#include <cassert>

std::string ShapesBase::materialsFolder;
ot::UID ShapesBase::materialsFolderID = 0;

ShapesBase::ShapesBase(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName, EntityCache *_entityCache, ClassFactory *_classFactory) :
	uiComponent(_uiComponent),
	modelComponent(_modelComponent),
	serviceID(_serviceID),
	serviceName(_serviceName),
	entityCache(_entityCache),
	classFactory(_classFactory)
{

}

std::string ShapesBase::to_string(double value)
{
	std::ostringstream out;
	out.precision(14);
	out << value;
	return out.str();
}

void ShapesBase::deleteNonStandardProperties(EntityGeometry* geometryEntity)
{
	std::list<EntityPropertiesBase *> propertyList = geometryEntity->getProperties().getListOfAllProperties();

	for (auto prop : propertyList)
	{
		if (prop->getGroup() != "Appearance" && prop->getGroup() != "Transformation" && prop->getGroup() != "Solver" && prop->getGroup() != "Mesh" && prop->getGroup() != "Mesh refinement")
		{
			// We do have a property belonging to a non-standard group
			geometryEntity->getProperties().deleteProperty(prop->getName(), prop->getGroup());
		}
	}
}

void ShapesBase::storeShapeInModel(const TopoDS_Shape & _shape, std::vector<double> &_transform, const std::string& _name, 
									  const std::string &_type, std::list<std::pair<std::string, std::string>> &shapeParameters, std::list<std::string> &faceNames)
{
	// Here we set some defaults (the color should be taken from the settings)
	ApplicationSettings * settings = ApplicationSettings::instance();

	int colorR = settings->geometryDefaultColor.r();
	int colorG = settings->geometryDefaultColor.g();
	int colorB = settings->geometryDefaultColor.b();

	if (materialsFolder.empty())
	{
		// The materials folder information has not yet been retrieved. We get the information about the entity from the model
		materialsFolder = "Materials";

		std::list<std::string> entityList{materialsFolder};
		std::list<ot::EntityInformation> entityInfo;

		modelComponent->getEntityInformation(entityList, entityInfo);

		assert(entityInfo.size() == 1);
		assert(entityInfo.front().getEntityName() == materialsFolder);

		materialsFolderID = entityInfo.front().getEntityID();
	}

	// Find the parent folder name of the entity
	std::string parentFolder;
	size_t index = _name.rfind('/');

	if (index != std::string::npos)
	{
		parentFolder = _name.substr(0, index);
	}

	// Get the list of items in the parent folder
	std::list<std::string> folderItems = modelComponent->getListOfFolderItems(parentFolder);

	// Now make sure that the new name of the item is unique
	std::string itemName = _name;
	if (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end())
	{
		int count = 1;
		do
		{
			itemName = _name + "_" + std::to_string(count);
			count++;
		} while (std::find(folderItems.begin(), folderItems.end(), itemName) != folderItems.end());
	}

	// Transform the brep according to the given transformation
	gp_Trsf transform;

	assert(_transform.size() == 16);
	transform.SetValues(_transform[ 0], _transform[ 1], _transform[ 2], _transform[ 3],
						_transform[ 4], _transform[ 5], _transform[ 6], _transform[ 7],
						_transform[ 8], _transform[ 9], _transform[10], _transform[11]);
	assert(_transform[12] == 0.0);
	assert(_transform[13] == 0.0);
	assert(_transform[14] == 0.0);
	assert(_transform[15] == 1.0);

	BRepBuilderAPI_Transform shapeTransform(transform);
	shapeTransform.Perform(_shape, false);

	TopoDS_Shape shape = shapeTransform.Shape();

	// Now get a new entity ID for creating the new geometry item
	ot::UID entityID = modelComponent->createEntityUID();
	ot::UID brepID   = modelComponent->createEntityUID();
	ot::UID facetsID = modelComponent->createEntityUID();

	// Let's create the new geometry item
	EntityGeometry *geometryEntity = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, serviceName);
	geometryEntity->setName(itemName);
	geometryEntity->setEditable(true);
	geometryEntity->setBrep(shape);
	geometryEntity->getBrepEntity()->setTransform(transform);

	applyFaceNames(geometryEntity, shape, faceNames);

	EntityPropertiesString *typeProp = new EntityPropertiesString("shapeType", _type);
	typeProp->setVisible(false);
	geometryEntity->getProperties().createProperty(typeProp, "Internal");

	geometryEntity->createMaterialPropertiesOnly(colorR, colorG, colorB, materialsFolder, materialsFolderID);

	for (auto prop : shapeParameters)
	{
		EntityPropertiesString::createProperty("Dimensions", prop.first, prop.second, "", geometryEntity->getProperties());

		double value = geometryEntity->evaluateExpressionDouble(prop.second);

		// Create the double property which holds the current value of the expression
		EntityPropertiesDouble *doubleProp = new EntityPropertiesDouble("#" + prop.first, value);
		doubleProp->setVisible(false);

		geometryEntity->getProperties().createProperty(doubleProp, "Dimensions");
	}

	geometryEntity->createNonMaterialProperties();
	geometryEntity->setTransformation(_transform);

	// The geometry entity has two children: brep and facets. We need to assign ids to both of them (since we do not have a model state object here)
	geometryEntity->getBrepEntity()->setEntityID(brepID);
	geometryEntity->getFacets()->setEntityID(facetsID);

	// Now we facet the entity and store it to the data base afterward.
	geometryEntity->facetEntity(false);
	geometryEntity->StoreToDataBase();

	ot::UID entityVersion = geometryEntity->getEntityStorageVersion();
	ot::UID brepVersion   = geometryEntity->getBrepEntity()->getEntityStorageVersion();
	ot::UID facetsVersion = geometryEntity->getFacets()->getEntityStorageVersion();

	entityCache->cacheEntity(geometryEntity->getBrepEntity());
	geometryEntity->detachBrep();

	entityCache->cacheEntity(geometryEntity);
	geometryEntity = nullptr;

	// Finally add the new entity to the model (data children need to come first)

	std::list<ot::UID> topologyEntityIDList = { entityID };
	std::list<ot::UID> topologyEntityVersionList = { entityVersion };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList = { brepID, facetsID};
	std::list<ot::UID> dataEntityVersionList = { brepVersion , facetsVersion };
	std::list<ot::UID> dataEntityParentList = { entityID, entityID };

	modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create new object: " + itemName);
}

void ShapesBase::applyFaceNames(EntityGeometry* geomEntity, const TopoDS_Shape& _shape, std::list<std::string>& faceNames)
{
	TopExp_Explorer exp;
	size_t faceCount = 0;
	for (exp.Init(_shape, TopAbs_FACE); exp.More(); exp.Next()) faceCount++;

	if (faceNames.size() != faceCount)
	{
		assert(0); // Inconsistent face naming information
	}
	else
	{
		for (exp.Init(_shape, TopAbs_FACE); exp.More(); exp.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(exp.Current());

			geomEntity->getBrepEntity()->setFaceName(aFace, faceNames.front());
			faceNames.pop_front();
		}
	}
}

void ShapesBase::writeShapeToStepFile(const TopoDS_Shape & _shape, const std::string& _filename) 
{
	// Transfer shape to step writer
	STEPControl_Writer stepWriter;
	auto transferStat = stepWriter.Transfer(_shape, STEPControl_StepModelType::STEPControl_ManifoldSolidBrep);
	switch (transferStat)
	{
	case IFSelect_RetVoid:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Transfer: Void");
		}
		return;
	case IFSelect_RetDone: break;
	case IFSelect_RetError:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Transfer: Error");
		}
		break;
	case IFSelect_RetFail:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Transfer: Fail");
		}
		return;
	case IFSelect_RetStop:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Transfer: Stop");
		}
		return;
	default:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Transfer: Unknown");
		}
		return;
	}

	// Write data

	auto stat = stepWriter.Write((_filename + ".stp").c_str());
	switch (stat)
	{
	case IFSelect_RetVoid:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Export: Void");
		}
		break;
	case IFSelect_RetDone:
		if (uiComponent) {
			std::string msg("Modeling: Export: Done\nSaved to file:\n");
			msg.append(_filename).append(".stp");
			uiComponent->displayInformationPrompt(msg);
		}
		break;
	case IFSelect_RetError:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Export: Error");
		}
		break;
	case IFSelect_RetFail:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Export: Fail");
		}
		break;
	case IFSelect_RetStop:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Export: Stop");
		}
		break;
	default:
		if (uiComponent) {
			uiComponent->displayWarningPrompt("Modeling: Export: Unknown");
		}
		break;
	}
}

