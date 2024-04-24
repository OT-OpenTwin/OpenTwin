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
#include "SimplifyRemoveFaces.h"
#include "ModalCommandHealing.h"

#include "STEPReader.h"

// Third party libraries
#include "base64.h"
#include "zlib.h"

// Open twin header
#include "OTCore/OTAssert.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/SettingsData.h"
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

void Application::run(void)
{
	// Add code that should be executed when the service is started and may start its work

	// We want to be connected to the database all the time
	if (!EnsureDataBaseConnection())
	{
		assert(0);  // Data base connection failed
	}
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	entityCache.setModelComponent(m_modelComponent);

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
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		std::list<ot::UID> entityIDs      = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		std::list<ot::UID> brepVersions   = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_BrepVersionList);
		bool itemsVisible			  = ot::json::getBool(_doc, OT_ACTION_PARAM_MODEL_ItemsVisible);

		std::list<ot::UID> modifiedEntities = getUpdateManager()->updateEntities(entityIDs, entityVersions, brepVersions, itemsVisible);

		getUpdateManager()->checkParentUpdates(modifiedEntities);

		entityCache.shrinkCache();
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

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
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

	_ui->addMenuButton("Modeling", "Repair", "Remove Faces", "Remove Faces", lockTypes, "RemoveFace", "Default", "");
	_ui->addMenuButton("Modeling", "Repair", "Heal", "Heal", lockTypes, "Healing", "Default", "");

	modelSelectionChanged();
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

void Application::modelSelectionChanged(void)
{
	uiComponent()->setControlState("Modeling:Modify:Boolean Add", !m_selectedEntities.empty());
	uiComponent()->setControlState("Modeling:Modify:Boolean Subtract", !m_selectedEntities.empty());
	uiComponent()->setControlState("Modeling:Modify:Boolean Intersect", !m_selectedEntities.empty());
	uiComponent()->setControlState("Modeling:Modify:Transform", !m_selectedEntities.empty());

	// We need to call the handler in the base class
	ApplicationBase::modelSelectionChanged();
}


ot::SettingsData * Application::createSettings(void) {
	ApplicationSettings * settings = ApplicationSettings::instance();
	ot::SettingsData * dataset = new ot::SettingsData("ModelingServiceSettings", "1.0");
	ot::SettingsGroup * root = dataset->addGroup("Modeling", "Modeling");

	ot::SettingsGroup * geom = root->addSubgroup("Geom", "Geometry");
	ot::SettingsItemColor * geomColor = new ot::SettingsItemColor("Color", "Geometry default color", settings->geometryDefaultColor);
	ot::SettingsItemSelection * geomNamingMode = new ot::SettingsItemSelection("NamingMode", "New geometry naming mode", { settings->geometryNamingModeGeometryBased, settings->geometryNamingModeCustom }, settings->geometryNamingMode);
	ot::SettingsItemString * geomCustomName = new ot::SettingsItemString("CustomName", "New geometry name", settings->geometryDefaultName, "Name of a new created geometry");
	
	geomCustomName->setVisible(geomNamingMode->selectedValue() == settings->geometryNamingModeCustom);
	
	geom->addItem(geomColor);
	geom->addItem(geomNamingMode);
	geom->addItem(geomCustomName);
	
	return dataset;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {
	ApplicationSettings * settings = ApplicationSettings::instance();

	ot::SettingsItemColor * geomColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Modeling:Geom:Color"));
	ot::SettingsItemSelection * geomNamingMode = dynamic_cast<ot::SettingsItemSelection *>(_dataset->findItemByLogicalName("Modeling:Geom:NamingMode"));
	ot::SettingsItemString * geomCustomName = dynamic_cast<ot::SettingsItemString *>(_dataset->findItemByLogicalName("Modeling:Geom:CustomName"));
	
	if (geomColor) { settings->geometryDefaultColor = geomColor->value(); } else { OTAssert(0, "Item not found"); }
	if (geomNamingMode) { settings->geometryNamingMode = geomNamingMode->selectedValue(); } else { OTAssert(0, "Item not found"); }
	if (geomCustomName) { settings->geometryDefaultName = geomCustomName->value(); } else { OTAssert(0, "Item not found"); }
	if (geomNamingMode && geomCustomName) { geomCustomName->setVisible(geomNamingMode->selectedValue() == settings->geometryNamingModeCustom); }
}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	std::string name = _item->logicalName();
	if (name == "Modeling:Geom:Color") {
		ot::SettingsItemColor * geomColor = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (geomColor == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryDefaultColor = geomColor->value();
	}
	else if (name == "Modeling:Geom:NamingMode") {
		ot::SettingsItemSelection * geomNamingMode = dynamic_cast<ot::SettingsItemSelection *>(_item);
		if (geomNamingMode == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryNamingMode = geomNamingMode->selectedValue();
		return true;
	}
	else if (name == "Modeling:Geom:CustomName") {
		ot::SettingsItemString * geomCustomName = dynamic_cast<ot::SettingsItemString *>(_item);
		if (geomCustomName == nullptr) {
			OTAssert(0, "Cast failed"); return false;
		}
		ApplicationSettings::instance()->geometryDefaultName = geomCustomName->value();
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
	m_modelComponent->getSelectedEntityInformation(entityInfo, "EntityGeometry");

	// Now we filter those which are under the "Geometry" folder
	std::list<ot::EntityInformation> entityInfoGeometry;

	std::string filter = "Geometry/";

	for (auto item : entityInfo)
	{
		if (item.getName().substr(0, filter.size()) == filter)
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
		primitiveManager = new PrimitiveManager(m_uiComponent, m_modelComponent, serviceName(), serviceID(), &entityCache, &getClassFactory());
	}

	return primitiveManager; 
}

BooleanOperations *Application::getBooleanOperations(void) 
{ 
	if (booleanOperations == nullptr)
	{
		booleanOperations = new BooleanOperations(m_uiComponent, m_modelComponent, serviceName(), &entityCache, serviceID());
	}

	return booleanOperations; 
}

UpdateManager *Application::getUpdateManager(void) 
{ 
	if (updateManager == nullptr)
	{
		updateManager = new UpdateManager(m_uiComponent, m_modelComponent, &entityCache, getPrimitiveManager(), getBooleanOperations(), &getClassFactory());
	}

	return updateManager; 
}

Transformations *Application::getTransformationManager(void) 
{ 
	if (transformationManager == nullptr)
	{
		transformationManager = new Transformations(m_uiComponent, m_modelComponent, serviceID(), serviceName(), &entityCache, &getClassFactory());
		transformationManager->setUpdateManager(getUpdateManager());	
	}

	return transformationManager; 
}

SimplifyRemoveFaces *Application::getRemoveFacesOperation(void)
{
	if (removeFaces == nullptr)
	{
		removeFaces = new SimplifyRemoveFaces(m_uiComponent, m_modelComponent, serviceID(), serviceName(), &entityCache, &getClassFactory());
		removeFaces->setUpdateManager(getUpdateManager());	
	}

	return removeFaces; 
}

STEPReader *Application::getSTEPReader(void)
{
	if (stepReader == nullptr)
	{
		stepReader = new STEPReader(this, serviceName());
	}

	return stepReader; 
}


void Application::importSTEP(void)
{
	// Get a file name for the STEP file from the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import STEP File", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString("STEP files (*.stp;*.step)", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importSTEPFile", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	uiComponent()->sendMessage(true, doc, tmp);
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
