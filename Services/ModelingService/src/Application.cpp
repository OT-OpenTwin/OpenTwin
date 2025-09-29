/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ApplicationSettings.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"
#include "DataBase.h"

#include "PrimitiveManager.h"
#include "PrimitiveCuboid.h"
#include "PrimitiveCylinder.h"
#include "PrimitiveCone.h"
#include "PrimitiveSphere.h"
#include "PrimitiveTorus.h"
#include "PrimitivePyramid.h"

#include "BooleanOperations.h"
#include "Transformations.h"
#include "UpdateManager.h"
#include "ChamferEdges.h"
#include "BlendEdges.h"
#include "SimplifyRemoveFaces.h"
#include "ModalCommandHealing.h"

#include "STEPReader.h"

// Third party libraries
#include "base64.h"
#include "zlib.h"

// Open twin header
#include "OTSystem/OTAssert.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/FileExtension.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_ModelingService

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_ModelingService

Application::Application() :
	ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier()),
	primitiveManager(nullptr),
	booleanOperations(nullptr),
	updateManager(nullptr),
	transformationManager(nullptr),
	removeFaces(nullptr),
	stepReader(nullptr)
{
	getClassFactory().SetNextHandler(&classFactoryCAD);
	classFactoryCAD.SetChainRoot(&(getClassFactory()));

	entityCache.setApplication(this);
}

Application::~Application()
{
	if (primitiveManager != nullptr)
	{
		delete primitiveManager; 
		primitiveManager = nullptr;
	}

	if (booleanOperations != nullptr)
	{
		delete booleanOperations; 
		booleanOperations = nullptr;
	}

	if (updateManager != nullptr)
	{
		delete updateManager; 
		updateManager = nullptr;
	}

	if (transformationManager != nullptr)
	{
		delete transformationManager; 
		transformationManager = nullptr;
	}

	if (removeFaces != nullptr)
	{
		delete removeFaces; 
		removeFaces = nullptr;
	}
}



// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	entityCache.setModelComponent(this->getModelComponent());

	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);

		if      (action == "Modeling:Create:Cuboid")            { getPrimitiveManager()->getCuboid()->sendRubberbandData(); }
		else if (action == "Modeling:Create:Cylinder")          { getPrimitiveManager()->getCylinder()->sendRubberbandData(); }
		else if (action == "Modeling:Create:Sphere")            { getPrimitiveManager()->getSphere()->sendRubberbandData(); }
		else if (action == "Modeling:Create:Torus")             { getPrimitiveManager()->getTorus()->sendRubberbandData(); }
		else if (action == "Modeling:Create:Cone")              { getPrimitiveManager()->getCone()->sendRubberbandData(); }
		else if (action == "Modeling:Create:Pyramid")           { getPrimitiveManager()->getPyramid()->sendRubberbandData(); }
		else if (action == "Modeling:Modify:Boolean Add")       { getBooleanOperations()->enterAddMode(getSelectedGeometryEntities()); }
		else if (action == "Modeling:Modify:Boolean Subtract")  { getBooleanOperations()->enterSubtractMode(getSelectedGeometryEntities()); }
		else if (action == "Modeling:Modify:Boolean Intersect") { getBooleanOperations()->enterIntersectMode(getSelectedGeometryEntities()); }
		else if (action == "Modeling:Modify:Transform")         { getTransformationManager()->enterTransformMode(getSelectedGeometryEntities()); }
		else if (action == "Modeling:Modify:Chamfer Edges")     { getChamferEdgesManager()->enterSelectEdgesMode(); }
		else if (action == "Modeling:Modify:Blend Edges")       { getBlendEdgesManager()->enterSelectEdgesMode(); }
		else if (action == "Modeling:Repair:Remove Faces")      { getRemoveFacesOperation()->enterRemoveFacesMode(); }
		else if (action == "Modeling:Repair:Heal")			    { new ModalCommandHealing(this, "Modeling", "Modeling:Repair:Heal"); }
		else if (action == "Modeling:Import:STEP")			    { importSTEP(); }
		else assert(0); // Unhandled button action
	}
	else if (_action == OT_ACTION_CMD_MODEL_CreateGeometryFromRubberbandData) 
	{
		std::string note              = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note);
		std::string json              = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW_RUBBERBAND_PointDocument);
		std::vector<double> transform = ot::json::getDoubleVector(_doc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Transform);

		getPrimitiveManager()->createFromRubberbandJson(note, json, transform);
	}
	else if (_action == OT_ACTION_CMD_MODEL_EntitiesSelected)
	{
		std::string selectionAction         = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_SelectionAction);
		std::string selectionInfo           = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_SelectionInfo);
		std::list<std::string> optionNames  = ot::json::getStringList(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
		std::list<std::string> optionValues = ot::json::getStringList(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);
		
		// Build a map from the option name and values lists
		std::map<std::string, std::string> options;
		auto optValue = optionValues.begin();
		for (auto optName : optionNames)
		{
			options[optName] = *optValue;
			optValue++;
		}

		if (selectionAction == "TRANSFORM")
		{
			getTransformationManager()->transformEntities(selectionInfo, options);
		}
		else if (selectionAction == "BOOLEAN_ADD" || selectionAction == "BOOLEAN_SUBTRACT" || selectionAction == "BOOLEAN_INTERSECT")
		{
			getBooleanOperations()->perfromOperationForSelectedEntities(selectionAction, selectionInfo, options);
		}
		else if (selectionAction == "REMOVE_FACE")
		{
			getRemoveFacesOperation()->performOperation(selectionInfo);
		}
		else if (selectionAction == "CHAMFER_EDGE")
		{
			getChamferEdgesManager()->performOperation(selectionInfo);
		}
		else if (selectionAction == "BLEND_EDGE")
		{
			getBlendEdgesManager()->performOperation(selectionInfo);
		}
		else
		{
			assert(0); // Unhandled action
		}
	}
	else if (_action == OT_ACTION_CMD_MODEL_ExecuteFunction)
	{
		std::string function = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
		std::string mode = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mode);
		std::string originalName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_OriginalName);

		if (mode == OT_ACTION_VALUE_FILE_Mode_Name)
		{
			std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);
			executeFunction(function, fileName, false, originalName);
		}
		else if (mode == OT_ACTION_VALUE_FILE_Mode_Content)
		{
			std::string content        = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
			ot::UID uncompressedDataLength = ot::json::getUInt64(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

			// Create a tmp file from uncompressing the data
			std::string tmpFileName = CreateTmpFileFromCompressedData(content, uncompressedDataLength);

			// Process the file content
			executeFunction(function, tmpFileName, true, originalName);
		}
	}

	entityCache.shrinkCache();

	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	ot::LockTypeFlags lockTypes;
	lockTypes.setFlag(ot::LockModelWrite);
	lockTypes.setFlag(ot::LockViewWrite);
	lockTypes.setFlag(ot::LockModelRead);

	_ui->addMenuPage("Modeling");
	_ui->addMenuGroup("Modeling", "Import");
	_ui->addMenuGroup("Modeling", "Create");
	_ui->addMenuGroup("Modeling", "Modify");
	_ui->addMenuGroup("Modeling", "Repair");

	_ui->addMenuButton("Modeling", "Import", "STEP", "STEP", lockTypes, "Import", "Default", "Ctrl+O");

	_ui->addMenuButton("Modeling", "Create", "Cuboid", "Cuboid", lockTypes, "Cuboid", "Default", "Ctrl+Alt+1");
	_ui->addMenuButton("Modeling", "Create", "Cylinder", "Cylinder", lockTypes, "Cylinder", "Default", "Ctrl+Alt+2");
	_ui->addMenuButton("Modeling", "Create", "Sphere", "Sphere", lockTypes, "Sphere", "Default", "Ctrl+Alt+4");
	_ui->addMenuButton("Modeling", "Create", "Torus", "Torus", lockTypes, "Torus", "Default", "Ctrl+Alt+5");
	_ui->addMenuButton("Modeling", "Create", "Cone", "Cone", lockTypes, "Cone", "Default", "Ctrl+Alt+6");
	_ui->addMenuButton("Modeling", "Create", "Pyramid", "Pyramid", lockTypes, "Pyramid", "Default", "Ctrl+Alt+7");

	_ui->addMenuButton("Modeling", "Modify", "Boolean Add", "Boolean Add", lockTypes, "BooleanAdd", "Default", "");
	_ui->addMenuButton("Modeling", "Modify", "Boolean Subtract", "Boolean Subtract", lockTypes, "BooleanSubtract", "Default", "");
	_ui->addMenuButton("Modeling", "Modify", "Boolean Intersect", "Boolean Intersect", lockTypes, "BooleanIntersect", "Default", "");

	_ui->addMenuButton("Modeling", "Modify", "Transform", "Transform", lockTypes, "Transform", "Default", "");
	_ui->addMenuButton("Modeling", "Modify", "Chamfer Edges", "Chamfer Edges", lockTypes, "ChamferEdges", "Default", "");
	_ui->addMenuButton("Modeling", "Modify", "Blend Edges", "Blend Edges", lockTypes, "BlendEdges", "Default", "");

	_ui->addMenuButton("Modeling", "Repair", "Remove Faces", "Remove Faces", lockTypes, "RemoveFace", "Default", "");
	_ui->addMenuButton("Modeling", "Repair", "Heal", "Heal", lockTypes, "Healing", "Default", "");

	modelSelectionChanged();
}

void Application::modelSelectionChanged(void)
{
	const bool buttonsEnabled = !this->getSelectedEntities().empty();

	getUiComponent()->setControlState("Modeling:Modify:Boolean Add", buttonsEnabled);
	getUiComponent()->setControlState("Modeling:Modify:Boolean Subtract", buttonsEnabled);
	getUiComponent()->setControlState("Modeling:Modify:Boolean Intersect", buttonsEnabled);
	getUiComponent()->setControlState("Modeling:Modify:Transform", buttonsEnabled);

	// We need to call the handler in the base class
	ApplicationBase::modelSelectionChanged();
}

void Application::propertyChanged(ot::JsonDocument& _doc)
{
	std::list<ot::UID> entityIDs = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
	std::list<ot::UID> entityVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	std::list<ot::UID> brepVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_BrepVersionList);
	bool itemsVisible = ot::json::getBool(_doc, OT_ACTION_PARAM_MODEL_ItemsVisible);

	std::list<ot::UID> modifiedEntities = getUpdateManager()->updateEntities(entityIDs, entityVersions, brepVersions, itemsVisible);

	getUpdateManager()->checkParentUpdates(modifiedEntities);

	entityCache.shrinkCache();
}


ot::PropertyGridCfg Application::createSettings(void) const {
	ApplicationSettings* settings = ApplicationSettings::instance();

	ot::PropertyGridCfg config;

	ot::PropertyGroup* geom = new ot::PropertyGroup("Geometry");
	ot::PropertyColor* geomColor = new ot::PropertyColor("Default Geometry Color", settings->geometryDefaultColor);
	ot::PropertyStringList* geomNamingMode = new ot::PropertyStringList("Geometry Naming Mode", settings->geometryNamingMode, std::list<std::string>({ settings->geometryNamingModeGeometryBased, settings->geometryNamingModeCustom }));
	geomNamingMode->setPropertyTip("Naming mode for new geometries");
	ot::PropertyString* geomCustomName = new ot::PropertyString("New Geometry Name", settings->geometryDefaultName);

	// Visibility
	geomCustomName->setPropertyFlag(ot::Property::IsHidden, geomNamingMode->getCurrent() != settings->geometryNamingModeCustom);
	
	// Add
	geom->addProperty(geomColor);
	geom->addProperty(geomNamingMode);
	geom->addProperty(geomCustomName);
	
	config.addRootGroup(geom);

	return config;
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {
	ApplicationSettings * settings = ApplicationSettings::instance();

	const ot::PropertyColor* geomColor = dynamic_cast<const ot::PropertyColor*>(_dataset.findPropertyByPath("Geometry/Default Geometry Color"));
	const ot::PropertyStringList* geomNamingMode = dynamic_cast<const ot::PropertyStringList*>(_dataset.findPropertyByPath("Geometry/Geometry Naming Mode"));
	const ot::PropertyString* geomCustomName = dynamic_cast<const ot::PropertyString*>(_dataset.findPropertyByPath("Geometry/New Geometry Name"));
	
	if (geomColor) { settings->geometryDefaultColor = geomColor->getValue(); } else { OTAssert(0, "Item not found"); }
	if (geomNamingMode) { settings->geometryNamingMode = geomNamingMode->getCurrent(); } else { OTAssert(0, "Item not found"); }
	if (geomCustomName) { settings->geometryDefaultName = geomCustomName->getValue(); } else { OTAssert(0, "Item not found"); }
}

bool Application::settingChanged(const ot::Property * _item) {
	std::string name = _item->getPropertyPath();

	if (name == "Geometry/Default Geometry Color") {
		const ot::PropertyColor * geomColor = dynamic_cast<const ot::PropertyColor*>(_item);
		if (geomColor == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryDefaultColor = geomColor->getValue();
	}
	else if (name == "Geometry/Geometry Naming Mode") {
		const ot::PropertyStringList* geomNamingMode = dynamic_cast<const ot::PropertyStringList*>(_item);
		if (geomNamingMode == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryNamingMode = geomNamingMode->getCurrent();
		return true;
	}
	else if (name == "Geometry/New Geometry Name") {
		const ot::PropertyString* geomCustomName = dynamic_cast<const ot::PropertyString*>(_item);
		if (geomCustomName == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryDefaultName = geomCustomName->getValue();
	}
	else {
		OTAssert(0, "Unknown item name");
	}
	return false;
}

// ##################################################################################################################################

std::list<ot::EntityInformation> Application::getSelectedGeometryEntities(void)
{
	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getSelectedEntityInformation(entityInfo, "EntityGeometry");

	// Now we filter those which are under the "Geometry" folder
	std::list<ot::EntityInformation> entityInfoGeometry;

	std::string filter = "Geometry/";

	for (auto item : entityInfo)
	{
		if (item.getEntityName().substr(0, filter.size()) == filter)
		{
			// Here we have a geometry entity under the Geometry folder
			entityInfoGeometry.push_back(item);
		}
	}

	return entityInfoGeometry;
}

PrimitiveManager *Application::getPrimitiveManager(void) 
{ 
	if (primitiveManager == nullptr)
	{
		primitiveManager = new PrimitiveManager(this->getUiComponent(), this->getModelComponent(), getServiceName(), getServiceID(), &entityCache, &getClassFactory());
	}

	return primitiveManager; 
}

BooleanOperations *Application::getBooleanOperations(void) 
{ 
	if (booleanOperations == nullptr)
	{
		booleanOperations = new BooleanOperations(this->getUiComponent(), this->getModelComponent(), getServiceName(), &entityCache, getServiceID(), &getClassFactory());
	}

	return booleanOperations; 
}

UpdateManager *Application::getUpdateManager(void) 
{ 
	if (updateManager == nullptr)
	{
		updateManager = new UpdateManager(this->getUiComponent(), this->getModelComponent(), &entityCache, getPrimitiveManager(), getBooleanOperations(), getChamferEdgesManager(), getBlendEdgesManager(), &getClassFactory());
	}

	return updateManager; 
}

Transformations *Application::getTransformationManager(void) 
{ 
	if (transformationManager == nullptr)
	{
		transformationManager = new Transformations(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache, &getClassFactory());
		transformationManager->setUpdateManager(getUpdateManager());	
	}

	return transformationManager; 
}

ChamferEdges* Application::getChamferEdgesManager(void)
{
	if (chamferEdges == nullptr)
	{
		chamferEdges = new ChamferEdges(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache, &getClassFactory());
		chamferEdges->setUpdateManager(getUpdateManager());
	}

	return chamferEdges;
}

BlendEdges* Application::getBlendEdgesManager(void)
{
	if (blendEdges == nullptr)
	{
		blendEdges = new BlendEdges(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache, &getClassFactory());
		blendEdges->setUpdateManager(getUpdateManager());
	}

	return blendEdges;
}


SimplifyRemoveFaces *Application::getRemoveFacesOperation(void)
{
	if (removeFaces == nullptr)
	{
		removeFaces = new SimplifyRemoveFaces(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache, &getClassFactory());
		removeFaces->setUpdateManager(getUpdateManager());	
	}

	return removeFaces; 
}

STEPReader *Application::getSTEPReader(void)
{
	if (stepReader == nullptr)
	{
		stepReader = new STEPReader(this, getServiceName());
	}

	return stepReader; 
}


void Application::importSTEP(void)
{
	// Get a file name for the STEP file from the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import STEP File", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(ot::FileExtension::toFilterString({ ot::FileExtension::Step, ot::FileExtension::AllFiles }), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importSTEPFile", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::executeFunction(const std::string &function, const std::string &fileName, bool removeFile, const std::string &originalName)
{
	if (function == "importSTEPFile")
	{
		getSTEPReader()->importSTEPFile(fileName, removeFile, originalName);
	}
	else assert(0); // Unhandled function action
}

std::string Application::CreateTmpFileFromCompressedData(const std::string &data, ot::UID uncompressedDataLength)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(data.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, data.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef *)decodedString, &destLen, (Bytef *)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	// Store the data in a temporary file
	std::string tmpFileName = DataBase::GetDataBase()->getTmpFileName();

	std::ofstream file(tmpFileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	return tmpFileName;
}
