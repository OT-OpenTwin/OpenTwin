/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "UiNotifier.h"
#include "Application.h"
#include "ModelNotifier.h"
#include "SubprocessManager.h"

// Open twin header
#include "OTCore/Variable.h"
#include "OTCore/TypeNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"
#include "OTGui/StyledTextBuilder.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "EntitySolverPyrit.h"
#include "EntityResultText.h"
#include "EntityFileText.h"
#include "EntityResultUnstructuredMeshVtk.h"
#include "EntityVisUnstructuredScalarSurface.h"
#include "EntityVisUnstructuredVectorSurface.h"

#include <fstream>

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_PYRIT, OT_INFO_SERVICE_TYPE_PYRIT, new UiNotifier(), new ModelNotifier()),
	m_subprocessManager(nullptr)
{
	m_addSolverButton = ot::ToolBarButtonCfg("Pyrit", "Solver", "Create Solver", "Default/AddSolver");
	m_addSolverButton.setButtonLockFlags(ot::LockType::ModelWrite);
	connectToolBarButton(m_addSolverButton, this, &Application::handleAddSolver);

	m_runSolverButton = ot::ToolBarButtonCfg("Pyrit", "Solver", "Run Solver", "Default/RunSolver");
	m_runSolverButton.setButtonLockFlags(ot::LockType::ModelWrite);
	connectToolBarButton(m_runSolverButton, this, &Application::handleRunSolver);
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions



// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (m_subprocessManager) {
		OT_LOG_EA("Application already in run mode");
		return;
	}

	// Create new subprocess manager
	m_subprocessManager = new SubprocessManager(this);

	DataBaseInfo info;
	info.setSiteID(this->getSiteID());
	info.setDataBaseUrl(DataBase::instance().getDataBaseServerURL());
	info.setCollectionName(this->getCollectionName());
	info.setUserName(DataBase::instance().getUserName());
	info.setUserPassword(DataBase::instance().getUserPassword());

	m_subprocessManager->setDataBaseInfo(info);

	if (this->getUiComponent()) {
		m_subprocessManager->setFrontendUrl(this->getUiComponent()->getServiceURL());
	}
	if (this->getModelComponent()) {
		m_subprocessManager->setModelUrl(this->getModelComponent()->getServiceURL());
	}
}

void Application::uiConnected(ot::components::UiComponent * _ui) {
	if (m_subprocessManager) {
		m_subprocessManager->setFrontendUrl(_ui->getServiceURL());
	}
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Pyrit");

	_ui->addMenuGroup("Pyrit", "Solver");
	//	_ui->addMenuGroup("Pyrit", "Sources");

	_ui->addMenuButton(m_addSolverButton);
	_ui->addMenuButton(m_runSolverButton);

	//	_ui->addMenuButton("Pyrit", "Sources", "Define Electrostatic Potential", "Define Electrostatic Potential", modelWrite, "DefinePotential", "Default");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui) {
	if (m_subprocessManager) {
		m_subprocessManager->setFrontendUrl("");
	}
}

void Application::modelConnected(ot::components::ModelComponent * _model) {
	if (m_subprocessManager) {
		m_subprocessManager->setModelUrl(_model->getServiceURL());
	}
}

void Application::modelDisconnected(const ot::components::ModelComponent * _model) {
	if (m_subprocessManager) {
		m_subprocessManager->setModelUrl("");
	}
}

void Application::logFlagsChanged(const ot::LogFlags& _flags) {
	if (!m_subprocessManager) {
		return;
	}

	if (!m_subprocessManager->isConnected()) {
		return;
	}

	OT_LOG_D("Updating log flags");

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetLogFlags, doc.GetAllocator()), doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(_flags, flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LogFlags, flagsArr, doc.GetAllocator());

	std::string response;
	m_subprocessManager->sendRequest(doc, response);
}

// ##################################################################################################################################################################################################################

void Application::modelSelectionChanged()
{
	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		if (this->getSelectedEntities().size() > 0)
		{
			enabled.push_back("Pyrit:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("Pyrit:Solver:Run Solver");
		}

		this->getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

void Application::handleAddSolver()
{
	if (this->getUiComponent() == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = ot::ModelServiceAPI::getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = this->getModelComponent()->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = "Solvers/Pyrit" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	std::string scriptFolderName, scriptName;
	ot::UID scriptFolderID{ 0 }, scriptID{ 0 };

	ot::ModelServiceAPI::getAvailableScripts(scriptFolderName, scriptFolderID, scriptName, scriptID);

	// Create the new solver item and store it in the data base
	EntitySolverPyrit* solverEntity = new EntitySolverPyrit(entityID, nullptr, nullptr, nullptr, getServiceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);
	solverEntity->createProperties(meshFolderName, meshFolderID, meshName, meshID, scriptFolderName, scriptFolderID, scriptName, scriptID);

	solverEntity->storeToDataBase();

	// Register the new solver item in the model
	std::list<ot::UID> topologyEntityIDList = { solverEntity->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { solverEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "create solver");
}

void Application::handleRunSolver()
{
	if (!m_subprocessManager)
	{
		this->getUiComponent()->displayMessage("\nERROR: Python subsystem is not initialized.\n");
		return;
	}

	if (this->getSelectedEntities().empty())
	{
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}


	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverRunMap;
	for (auto& entity : this->getSelectedEntityInfos())
	{
		if (entity.getEntityType() == "EntitySolverPyrit")
		{
			if (entity.getEntityName().substr(0, 8) == "Solvers/")
			{
				size_t index = entity.getEntityName().find('/', 8);
				if (index != std::string::npos)
				{
					solverRunMap[entity.getEntityName().substr(0, index - 1)] = true;
				}
				else
				{
					solverRunMap[entity.getEntityName()] = true;
				}
			}
		}
	}

	std::list<std::string> solverRunList;
	for (auto solver : solverRunMap)
	{
		solverRunList.push_back(solver.first);
	}

	if (solverRunList.empty())
	{
		if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		this->getUiComponent()->displayMessage("\nERROR: No solver item has been selected.\n");
		return;
	}

	// Now we retrieve information about the solver items
	std::list<ot::EntityInformation> solverInfo;
	ot::ModelServiceAPI::getEntityInformation(solverRunList, solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : solverInfo)
	{
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdsSolver);

	// Now read the solver objects for each solver
	std::list<std::string> meshNameList;
	std::map<std::string, EntityBase*> solverMap;
	for (auto info : solverInfo)
	{
		EntityBase* entity = ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion());
		solverMap[info.getEntityName()] = entity;
	}

	std::list<ot::EntityInformation> meshInfo;
	ot::ModelServiceAPI::getEntityChildInformation("Meshes", meshInfo, false);

	// Finally start the worker thread to run the solvers
	std::thread workerThread(&Application::solverThread, this, solverInfo, meshInfo, solverMap);
	workerThread.detach();

	//solverThread(solverInfo, meshInfo, solverMap);
}

void Application::solverThread(std::list<ot::EntityInformation> solverInfo, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase*> solverMap) {
	ot::LockTypes lock(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);

	this->getUiComponent()->lockUI(lock);

	for (auto solver : solverInfo)
	{
		runSingleSolver(solver, meshInfo, solverMap[solver.getEntityName()]);
	}

	this->getUiComponent()->unlockUI(lock);
}

void Application::runSingleSolver(ot::EntityInformation& solver, std::list<ot::EntityInformation>& meshInfo, EntityBase* solverEntity)
{
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	this->getUiComponent()->displayMessage("\nPyrit solver started: " + solverName + "\n\n");

	if (solverEntity == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
		return;
	}

	EntityPropertiesSelection* problemType = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Problem type"));
	assert(problemType != nullptr);

	if (problemType == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read problem type for solver.\n");
		return;
	}

	std::string command;

	if (problemType->getValue() == "Custom")
	{
		command = problemTypeScript(solverEntity);
	}
	else if (problemType->getValue() == "Electrostatics")
	{


	}
	else
	{
		this->getUiComponent()->displayMessage("ERROR: Unknown problem type.\n");
		return;
	}

	if (command.empty())
	{
		return;
	}


	//EntityPropertiesEntityList* mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	//assert(mesh != nullptr);

	//if (mesh == nullptr)
	//{
	//	this->getUiComponent()->displayMessage("ERROR: Unable to read mesh information for solver.\n");
	//	return;
	//}

	//// First check whether a mesh with the given ID exists
	//bool meshFound = false;
	//ot::UID meshEntityID = mesh->getValueID();

	//for (auto meshItem : meshInfo)
	//{
	//	if (meshItem.getEntityID() == meshEntityID)
	//	{
	//		meshFound = true;
	//		break;
	//	}
	//}

	//// If the mesh with the given ID does not exist anymore, we search for a mesh with the same name as the last selected one.
	//if (!meshFound)
	//{
	//	meshEntityID = 0;
	//	for (auto meshItem : meshInfo)
	//	{
	//		if (meshItem.getEntityName() == mesh->getValueName())
	//		{
	//			meshEntityID = meshItem.getEntityID();
	//			break;
	//		}
	//	}
	//}

	//if (meshEntityID == 0)
	//{
	//	this->getUiComponent()->displayMessage("ERROR: The specified mesh does not exist: " + mesh->getValueName() + "\n");
	//	return;
	//}

	deleteSingleSolverResults(solverEntity);

	//std::string command =   "import time\n"
	//						"for i in range(5):\n"
	//						"    print(f'Python schreibt: {i}')\n"
	//						"    time.sleep(1)";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(command, doc.GetAllocator()), doc.GetAllocator());

	m_subprocessManager->startLogging();

	std::string returnMessage;
	if (!m_subprocessManager->sendRequest(doc, returnMessage)) {
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to execute pyrit script").toJson();
	}

	ot::ReturnMessage returnValue = ot::ReturnMessage::fromJson(returnMessage);
	
	if (returnValue.getStatus() == ot::ReturnMessage::Ok)
	{
		std::string message = "\nPyrit solver successfully completed.\n";
		this->getUiComponent()->displayMessage(message);
		m_subprocessManager->addLogText(message);
	}
	else if (returnValue.getStatus() == ot::ReturnMessage::Failed)
	{
		ot::StyledTextBuilder message;
		message << "\n[" << ot::StyledText::Error << ot::StyledText::Bold << "ERROR" << ot::StyledText::ClearStyle << "] " << "Pyrit solver failed : (" << returnValue.getWhat() << ")\n";

		this->getUiComponent()->displayStyledMessage(message);
		m_subprocessManager->addLogText("ERROR: Pyrit solver failed : (" + returnValue.getWhat() + ")\n");
	}
	else
	{
		std::string message = "ERROR: Unknown return status: " + returnValue.getStatusString() + "\n";
		this->getUiComponent()->displayMessage(message);
		m_subprocessManager->addLogText(message);
	}
	 
	std::string logFileText;
	m_subprocessManager->endLogging(logFileText);

	// Store the output in a result item

	EntityResultText* text = this->getModelComponent()->addResultTextEntity(solver.getEntityName() + "/Output", logFileText);

	getModelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	// TEMPORARY: Read the result data file and create a new result entity
	std::ifstream file("result.vtu", std::ios::binary | std::ios::ate);
	int data_length = (int)file.tellg();

	if (data_length != -1)
	{
		file.seekg(0, std::ios::beg);

		char* fileData = new char[data_length + 1];
		file.read(fileData, data_length);
		fileData[data_length] = 0;

		addScalarResult("energy_density", fileData, data_length, solverEntity);
		addScalarResult("region IDs", fileData, data_length, solverEntity);
		addScalarResult("reluctivity", fileData, data_length, solverEntity);
		addScalarResult("vector_potential", fileData, data_length, solverEntity);

		addVectorResult("flux_density", fileData, data_length, solverEntity);
		addVectorResult("magnetic_field", fileData, data_length, solverEntity);

		delete[] fileData;
		fileData = nullptr;
	}

	// Store the newly created items in the data base
	this->getModelComponent()->storeNewEntities("added solver results");
}

void Application::addScalarResult(const std::string &resultName, char* fileData, int data_length, EntityBase* solverEntity)
{
	EntityBinaryData* vtkData = new EntityBinaryData(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	vtkData->setData(fileData, data_length + 1);
	vtkData->storeToDataBase();

	ot::UID vtkDataEntityID = vtkData->getEntityID();
	ot::UID vtkDataEntityVersion = vtkData->getEntityStorageVersion();

	EntityResultUnstructuredMeshVtk* vtkResult = new EntityResultUnstructuredMeshVtk(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	vtkResult->setData(resultName, EntityResultUnstructuredMeshVtk::SCALAR, vtkData);
	vtkResult->storeToDataBase();

	EntityVisUnstructuredScalarSurface* visualizationEntity = new EntityVisUnstructuredScalarSurface(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	visualizationEntity->setName(solverEntity->getName() + "/Results/" + resultName);
	visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_SCALAR);
	visualizationEntity->setEditable(true);
	visualizationEntity->setInitiallyHidden(true);

	visualizationEntity->createProperties();

	visualizationEntity->setSource(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion());

	visualizationEntity->storeToDataBase();

	getModelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(vtkDataEntityID, vtkDataEntityVersion, vtkResult->getEntityID());
	getModelComponent()->addNewDataEntity(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion(), visualizationEntity->getEntityID());

	delete visualizationEntity;
	visualizationEntity = nullptr;

	delete vtkResult;
	vtkResult = nullptr;
}

void Application::addVectorResult(const std::string& resultName, char* fileData, int data_length, EntityBase* solverEntity)
{
	EntityBinaryData* vtkData = new EntityBinaryData(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	vtkData->setData(fileData, data_length + 1);
	vtkData->storeToDataBase();

	ot::UID vtkDataEntityID = vtkData->getEntityID();
	ot::UID vtkDataEntityVersion = vtkData->getEntityStorageVersion();

	EntityResultUnstructuredMeshVtk* vtkResult = new EntityResultUnstructuredMeshVtk(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	vtkResult->setData(resultName, EntityResultUnstructuredMeshVtk::VECTOR, vtkData);
	vtkResult->storeToDataBase();

	EntityVisUnstructuredVectorSurface* visualizationEntity = new EntityVisUnstructuredVectorSurface(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_VisualizationService);
	visualizationEntity->setName(solverEntity->getName() + "/Results/" + resultName);
	visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_VECTOR);
	visualizationEntity->setEditable(true);
	visualizationEntity->setInitiallyHidden(true);

	visualizationEntity->createProperties();

	visualizationEntity->setSource(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion());

	visualizationEntity->storeToDataBase();

	getModelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(vtkDataEntityID, vtkDataEntityVersion, vtkResult->getEntityID());
	getModelComponent()->addNewDataEntity(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion(), visualizationEntity->getEntityID());

	delete visualizationEntity;
	visualizationEntity = nullptr;

	delete vtkResult;
	vtkResult = nullptr;
}

void Application::deleteSingleSolverResults(EntityBase* solverEntity)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(solverEntity->getName() + "/Results");
	entityNameList.push_back(solverEntity->getName() + "/Output");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);
}

std::string Application::problemTypeScript(EntityBase* solverEntity)
{
	EntityPropertiesEntityList* script = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Script"));
	assert(script != nullptr);

	if (script == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read script information for solver.\n");
		return "";
	}

	ot::EntityInformation scriptInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(script->getValueName(), scriptInfo))
	{
		if (script == nullptr)
		{
			this->getUiComponent()->displayMessage("ERROR: Unable to read script.\n");
			return "";
		}
	}

	EntityBase* entity = ot::EntityAPI::readEntityFromEntityIDandVersion(scriptInfo.getEntityID(), scriptInfo.getEntityVersion());

	if (entity == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read script.\n");
		return "";
	}

	EntityFileText* scriptEntity = dynamic_cast<EntityFileText*>(entity);

	if (scriptEntity == nullptr)
	{
		delete entity;
		this->getUiComponent()->displayMessage("ERROR: Unable to read script (wrong data type).\n");
		return "";
	}

	std::string command = scriptEntity->getText();

	return command;
}
