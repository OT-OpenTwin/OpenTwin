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

Application& Application::instance() {
	static Application g_instance;
	return g_instance;
}

// ##################################################################################################################################

// Required functions

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

	_ui->addMenuButton(m_buttonImportStep);

	_ui->addMenuButton(m_buttonCreateCuboid);
	_ui->addMenuButton(m_buttonCreateCylinder);
	_ui->addMenuButton(m_buttonCreateSphere);
	_ui->addMenuButton(m_buttonCreateTorus);
	_ui->addMenuButton(m_buttonCreateCone);
	_ui->addMenuButton(m_buttonCreatePyramid);

	_ui->addMenuButton(m_buttonBooleanAdd);
	_ui->addMenuButton(m_buttonBooleanSubtract);
	_ui->addMenuButton(m_buttonBooleanIntersect);

	_ui->addMenuButton(m_buttonTransform);
	_ui->addMenuButton(m_buttonChamferEdges);
	_ui->addMenuButton(m_buttonBlendEdges);

	_ui->addMenuButton(m_buttonRemoveFaces);
	_ui->addMenuButton(m_buttonHealing);

	modelSelectionChanged();
}

void Application::modelConnected(ot::components::ModelComponent* _model) {
	entityCache.setModelComponent(_model);
}

void Application::modelDisconnected(const ot::components::ModelComponent* _serviceInfo) {
	entityCache.setModelComponent(nullptr);
}

void Application::modelSelectionChanged(void)
{
	const bool buttonsEnabled = !this->getSelectedEntities().empty();

	getUiComponent()->setControlState(m_buttonBooleanAdd.getFullPath(), buttonsEnabled);
	getUiComponent()->setControlState(m_buttonBooleanSubtract.getFullPath(), buttonsEnabled);
	getUiComponent()->setControlState(m_buttonBooleanIntersect.getFullPath(), buttonsEnabled);
	getUiComponent()->setControlState(m_buttonTransform.getFullPath(), buttonsEnabled);

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

void Application::actionHandlingCompleted() {
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
		primitiveManager = new PrimitiveManager(this->getUiComponent(), this->getModelComponent(), getServiceName(), getServiceID(), &entityCache);
	}

	return primitiveManager; 
}

BooleanOperations *Application::getBooleanOperations(void) 
{ 
	if (booleanOperations == nullptr)
	{
		booleanOperations = new BooleanOperations(this->getUiComponent(), this->getModelComponent(), getServiceName(), &entityCache, getServiceID());
	}

	return booleanOperations; 
}

UpdateManager *Application::getUpdateManager(void) 
{ 
	if (updateManager == nullptr)
	{
		updateManager = new UpdateManager(this->getUiComponent(), this->getModelComponent(), &entityCache, getPrimitiveManager(), getBooleanOperations(), getChamferEdgesManager(), getBlendEdgesManager());
	}

	return updateManager; 
}

Transformations *Application::getTransformationManager(void) 
{ 
	if (transformationManager == nullptr)
	{
		transformationManager = new Transformations(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache);
		transformationManager->setUpdateManager(getUpdateManager());	
	}

	return transformationManager; 
}

ChamferEdges* Application::getChamferEdgesManager(void)
{
	if (chamferEdges == nullptr)
	{
		chamferEdges = new ChamferEdges(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache);
		chamferEdges->setUpdateManager(getUpdateManager());
	}

	return chamferEdges;
}

BlendEdges* Application::getBlendEdgesManager(void)
{
	if (blendEdges == nullptr)
	{
		blendEdges = new BlendEdges(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache);
		blendEdges->setUpdateManager(getUpdateManager());
	}

	return blendEdges;
}

SimplifyRemoveFaces *Application::getRemoveFacesOperation(void)
{
	if (removeFaces == nullptr)
	{
		removeFaces = new SimplifyRemoveFaces(this->getUiComponent(), this->getModelComponent(), getServiceID(), getServiceName(), &entityCache);
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
	std::string tmpFileName = DataBase::instance().getTmpFileName();

	std::ofstream file(tmpFileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	return tmpFileName;
}

void Application::handleImportSTEP(ot::JsonDocument& _document) {
	std::string mode = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mode);
	std::string originalName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);

	if (mode == OT_ACTION_VALUE_FILE_Mode_Name) {
		std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Name);
		getSTEPReader()->importSTEPFile(fileName, false, originalName);
	}
	else if (mode == OT_ACTION_VALUE_FILE_Mode_Content) {
		std::string content = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
		ot::UID uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

		// Create a tmp file from uncompressing the data
		std::string tmpFileName = CreateTmpFileFromCompressedData(content, uncompressedDataLength);

		// Process the file content
		getSTEPReader()->importSTEPFile(tmpFileName, true, originalName);
	}

}

void Application::handleCreateGeometryFromRubberband(ot::JsonDocument& _document) {
	std::string note = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note);
	std::string json = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_PointDocument);
	std::vector<double> transform = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Transform);

	getPrimitiveManager()->createFromRubberbandJson(note, json, transform);
}

void Application::handleEntitiesSelected(ot::JsonDocument& _document) {
	std::string selectionAction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_SelectionAction);
	std::string selectionInfo = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_SelectionInfo);
	std::list<std::string> optionNames = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
	std::list<std::string> optionValues = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);

	// Build a map from the option name and values lists
	std::map<std::string, std::string> options;
	auto optValue = optionValues.begin();
	for (auto optName : optionNames) {
		options[optName] = *optValue;
		optValue++;
	}

	if (selectionAction == "TRANSFORM") {
		getTransformationManager()->transformEntities(selectionInfo, options);
	}
	else if (selectionAction == "BOOLEAN_ADD" || selectionAction == "BOOLEAN_SUBTRACT" || selectionAction == "BOOLEAN_INTERSECT") {
		getBooleanOperations()->perfromOperationForSelectedEntities(selectionAction, selectionInfo, options);
	}
	else if (selectionAction == "REMOVE_FACE") {
		getRemoveFacesOperation()->performOperation(selectionInfo);
	}
	else if (selectionAction == "CHAMFER_EDGE") {
		getChamferEdgesManager()->performOperation(selectionInfo);
	}
	else if (selectionAction == "BLEND_EDGE") {
		getBlendEdgesManager()->performOperation(selectionInfo);
	}
	else {
		OT_LOG_EAS("Unsupported selection action \"" + selectionAction + "\" received.");
	}
}

void Application::handleRequestImportSTEP() {
	// Get a file name for the STEP file from the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import STEP File", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(ot::FileExtension::toFilterString({ ot::FileExtension::Step, ot::FileExtension::AllFiles }), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString("importSTEPFile", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::handleCreateCuboid() {
	getPrimitiveManager()->getCuboid()->sendRubberbandData();
}

void Application::handleCreateCylinder() {
	getPrimitiveManager()->getCylinder()->sendRubberbandData();
}

void Application::handleCreateSphere() {
	getPrimitiveManager()->getSphere()->sendRubberbandData();
}

void Application::handleCreateTorus() {
	getPrimitiveManager()->getTorus()->sendRubberbandData();
}

void Application::handleCreateCone() {
	getPrimitiveManager()->getCone()->sendRubberbandData();
}

void Application::handleCreatePyramid() {
	getPrimitiveManager()->getPyramid()->sendRubberbandData();
}

void Application::handleBooleanAdd() {
	getBooleanOperations()->enterAddMode(getSelectedGeometryEntities());
}

void Application::handleBooleanSubtract() {
	getBooleanOperations()->enterSubtractMode(getSelectedGeometryEntities());
}

void Application::handleBooleanIntersect() {
	getBooleanOperations()->enterIntersectMode(getSelectedGeometryEntities());
}

void Application::handleTransform() {
	getTransformationManager()->enterTransformMode(getSelectedGeometryEntities());
}

void Application::handleChamferEdges() {
	getChamferEdgesManager()->enterSelectEdgesMode();
}

void Application::handleBlendEdges() {
	getBlendEdgesManager()->enterSelectEdgesMode();
}

void Application::handleRemoveFaces() {
	getRemoveFacesOperation()->enterRemoveFacesMode();
}

void Application::handleHealing() {
	new ModalCommandHealing(this, "Modeling", "Modeling:Repair:Heal");
}

Application::Application() :
	ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier()),
	primitiveManager(nullptr),
	booleanOperations(nullptr),
	updateManager(nullptr),
	transformationManager(nullptr),
	removeFaces(nullptr),
	stepReader(nullptr),
	blendEdges(nullptr),
	chamferEdges(nullptr) 
{
	entityCache.setApplication(this);

	// Connect actions
	connectAction("importSTEPFile", this, &Application::handleImportSTEP);
	connectAction(OT_ACTION_CMD_MODEL_EntitiesSelected, this, &Application::handleEntitiesSelected);
	connectAction(OT_ACTION_CMD_MODEL_CreateGeometryFromRubberbandData, this, &Application::handleCreateGeometryFromRubberband);

	// Setup and connect buttons
	ot::LockTypeFlags lockTypes = ot::LockModelWrite | ot::LockViewWrite | ot::LockModelRead;

	m_buttonImportStep = ot::ToolBarButtonCfg("Modeling", "Import", "Import STEP", "Default/Import");
	m_buttonImportStep.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+O");
	connectToolBarButton(m_buttonImportStep, this, &Application::handleRequestImportSTEP);

	m_buttonCreateCuboid = ot::ToolBarButtonCfg("Modeling", "Create", "Cuboid", "Default/Cuboid");
	m_buttonCreateCuboid.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+1");
	connectToolBarButton(m_buttonCreateCuboid, this, &Application::handleCreateCuboid);

	m_buttonCreateCylinder = ot::ToolBarButtonCfg("Modeling", "Create", "Cylinder", "Default/Cylinder");
	m_buttonCreateCylinder.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+2");
	connectToolBarButton(m_buttonCreateCylinder, this, &Application::handleCreateCylinder);

	m_buttonCreateSphere = ot::ToolBarButtonCfg("Modeling", "Create", "Sphere", "Default/Sphere");
	m_buttonCreateSphere.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+4");
	connectToolBarButton(m_buttonCreateSphere, this, &Application::handleCreateSphere);

	m_buttonCreateTorus = ot::ToolBarButtonCfg("Modeling", "Create", "Torus", "Default/Torus");
	m_buttonCreateTorus.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+5");
	connectToolBarButton(m_buttonCreateTorus, this, &Application::handleCreateTorus);

	m_buttonCreateCone = ot::ToolBarButtonCfg("Modeling", "Create", "Cone", "Default/Cone");
	m_buttonCreateCone.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+6");
	connectToolBarButton(m_buttonCreateCone, this, &Application::handleCreateCone);

	m_buttonCreatePyramid = ot::ToolBarButtonCfg("Modeling", "Create", "Pyramid", "Default/Pyramid");
	m_buttonCreatePyramid.setButtonLockFlags(lockTypes).setButtonKeySequence("Ctrl+Alt+7");
	connectToolBarButton(m_buttonCreatePyramid, this, &Application::handleCreatePyramid);

	m_buttonBooleanAdd = ot::ToolBarButtonCfg("Modeling", "Modify", "Boolean Add", "Default/BooleanAdd");
	m_buttonBooleanAdd.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonBooleanAdd, this, &Application::handleBooleanAdd);

	m_buttonBooleanSubtract = ot::ToolBarButtonCfg("Modeling", "Modify", "Boolean Subtract", "Default/BooleanSubtract");
	m_buttonBooleanSubtract.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonBooleanSubtract, this, &Application::handleBooleanSubtract);

	m_buttonBooleanIntersect = ot::ToolBarButtonCfg("Modeling", "Modify", "Boolean Intersect", "Default/BooleanIntersect");
	m_buttonBooleanIntersect.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonBooleanIntersect, this, &Application::handleBooleanIntersect);

	m_buttonTransform = ot::ToolBarButtonCfg("Modeling", "Modify", "Transform", "Default/Transform");
	m_buttonTransform.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonTransform, this, &Application::handleTransform);

	m_buttonChamferEdges = ot::ToolBarButtonCfg("Modeling", "Modify", "Chamfer Edges", "Default/ChamferEdges");
	m_buttonChamferEdges.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonChamferEdges, this, &Application::handleChamferEdges);

	m_buttonBlendEdges = ot::ToolBarButtonCfg("Modeling", "Modify", "Blend Edges", "Default/BlendEdges");
	m_buttonBlendEdges.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonBlendEdges, this, &Application::handleBlendEdges);

	m_buttonRemoveFaces = ot::ToolBarButtonCfg("Modeling", "Repair", "Remove Faces", "Default/RemoveFace");
	m_buttonRemoveFaces.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonRemoveFaces, this, &Application::handleRemoveFaces);

	m_buttonHealing = ot::ToolBarButtonCfg("Modeling", "Repair", "Heal", "Default/Healing");
	m_buttonHealing.setButtonLockFlags(lockTypes);
	connectToolBarButton(m_buttonHealing, this, &Application::handleHealing);
}

Application::~Application() {
	if (primitiveManager != nullptr) {
		delete primitiveManager;
		primitiveManager = nullptr;
	}

	if (booleanOperations != nullptr) {
		delete booleanOperations;
		booleanOperations = nullptr;
	}

	if (updateManager != nullptr) {
		delete updateManager;
		updateManager = nullptr;
	}

	if (transformationManager != nullptr) {
		delete transformationManager;
		transformationManager = nullptr;
	}

	if (removeFaces != nullptr) {
		delete removeFaces;
		removeFaces = nullptr;
	}
}
