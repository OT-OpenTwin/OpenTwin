// @otlicense
// File: Application.cpp
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

// Service header
#include "UiNotifier.h"
#include "Application.h"
#include "ModelNotifier.h"
#include "SubprocessManager.h"
#include "FDTDSolver.h"

// Open twin header
#include "OTCore/Variable/Variable.h"
#include "OTCore/TypeNames.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTGui/Style/StyledTextBuilder.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntitySolverOpenEMS.h"
#include "OTModelEntities/EntityResultText.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/EntityResultUnstructuredMeshVtk.h"
#include "OTModelEntities/EntityVisUnstructuredScalarSurface.h"
#include "OTModelEntities/EntityVisUnstructuredVectorSurface.h"
#include "OTModelEntities/EntityMeshCartesian.h"
#include "OTSystem/OperatingSystem.h"

#include <fstream>
#include <direct.h>
#include <windows.h>

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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_OPENEMS, OT_INFO_SERVICE_TYPE_OPENEMS, new UiNotifier(), new ModelNotifier()),
	m_subprocessManager(nullptr)
{
	m_addSolverButton = ot::ToolBarButtonCfg("OpenEMS", "Solver", "Create Solver", "Default/AddSolver");
	m_addSolverButton.setButtonLockFlags(ot::LockType::ModelWrite);
	connectToolBarButton(m_addSolverButton, this, &Application::handleAddSolver);

	m_runSolverButton = ot::ToolBarButtonCfg("OpenEMS", "Solver", "Run Solver", "Default/RunSolver");
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
	_ui->addMenuPage("OpenEMS");

	_ui->addMenuGroup("OpenEMS", "Solver");
	//	_ui->addMenuGroup("OpenEMS", "Sources");

	_ui->addMenuButton(m_addSolverButton);
	_ui->addMenuButton(m_runSolverButton);

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
			enabled.push_back("OpenEMS:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("OpenEMS:Solver:Run Solver");
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
		solverName = "Solvers/OpenEMS" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	std::string meshFolderName, meshName;
	ot::UID meshFolderID{ 0 }, meshID{ 0 };

	ot::ModelServiceAPI::getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverOpenEMS* solverEntity = new EntitySolverOpenEMS(entityID, nullptr, nullptr, nullptr);
	solverEntity->setName(solverName);
	solverEntity->setTreeItemEditable(true);
	solverEntity->createProperties(meshFolderName, meshFolderID, meshName, meshID);
	solverEntity->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		getServiceName()
	);

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
		if (entity.getEntityType() == "EntitySolverOpenEMS")
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

	for (auto solver : solverMap)
	{
		delete solver.second;
	}

	this->getUiComponent()->unlockUI(lock);
}

void Application::runSingleSolver(ot::EntityInformation& solver, std::list<ot::EntityInformation>& meshInfo, EntityBase* solverEntity)
{
	// Load the solver entity
	std::string solverName = solver.getEntityName();
	if (solverName.substr(0, 8) == "Solvers/")
	{
		solverName = solverName.substr(8);
	}

	if (this->getUiComponent() == nullptr) { assert(0); throw std::exception("UI is not connected"); }
	this->getUiComponent()->displayMessage("\nOpenEMS solver started: " + solverName + "\n\n");

	if (solverEntity == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
		return;
	}

	// Load the mesh entity
	EntityPropertiesEntityList* mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);

	if (mesh == nullptr)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to read mesh information for solver.\n");
		return;
	}

	// Update and load the mesh, if possible
	std::unique_ptr<EntityMeshCartesian> meshEntity(updateAndLoadMeshEntity(mesh->getValueName(), meshInfo));
	if (meshEntity == nullptr) return;

	// Delete previous solver results
	deleteSingleSolverResults(solverEntity);

	// Create the temp file for running the solver
	std::string tempDirPath = getUniqueTempDir();

	if (tempDirPath.empty())
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to create temporary working directory (TMP environment variable needs to be set).\n");
		return;
	}

	if (_mkdir(tempDirPath.c_str()) == -1)
	{
		this->getUiComponent()->displayMessage("ERROR: Unable to create temporary working directory (TMP environment variable needs to be set).\n");
		return;
	}

	// Start logging and solver execution
	m_subprocessManager->startLogging();

	EntityPropertiesBoolean* debug = dynamic_cast<EntityPropertiesBoolean*>(solverEntity->getProperties().getProperty("Debug"));
	assert(debug != nullptr);

	bool debugFlag = false;
	if (debug != nullptr) debugFlag = debug->getValue();

	FDTDSolver fdtdSolver(this, solverEntity, meshEntity.get(), getOpenEMSDir(), tempDirPath);

	std::string returnMessage;
	std::string solverCommand;

	try
	{
		solverCommand = fdtdSolver.generateRunCommand();

		if (debugFlag)
		{
			std::ofstream inputCommand(tempDirPath + "\\input.py");
			inputCommand << solverCommand;
			inputCommand.close();
		}

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(solverCommand, doc.GetAllocator()), doc.GetAllocator());

		if (!m_subprocessManager->sendRequest(doc, returnMessage)) {
			returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to execute OpenEMS script").toJson();
		}
	}
	catch (std::string& error)
	{
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, error).toJson();
	}

	ot::ReturnMessage returnValue = ot::ReturnMessage::fromJson(returnMessage);
	
	bool success = true;

	if (returnValue.getStatus() == ot::ReturnMessage::Ok)
	{
		std::string message = "\nOpenEMS solver successfully completed.\n";
		m_subprocessManager->addLogText(message, true);
	}
	else if (returnValue.getStatus() == ot::ReturnMessage::Failed)
	{
		ot::StyledTextBuilder message;
		message << "\n[" << ot::StyledText::Error << ot::StyledText::Bold << "ERROR" << ot::StyledText::ClearStyle << "] " << "OpenEMS solver failed : (" << returnValue.getWhat() << ")\n";

		m_subprocessManager->addLogText("ERROR: OpenEMS solver failed : (" + returnValue.getWhat() + ")\n", true);
		success = false;
	}
	else
	{
		std::string message = "ERROR: Unknown return status: " + returnValue.getStatusString() + "\n";
		m_subprocessManager->addLogText(message, true);
		success = false;
	}
	 
	// Convert and store the results
	if (success)
	{
		fdtdSolver.convertAndStoreResults();
	}

	// Remove the temp dir if requested
	if (debugFlag)
	{
		m_subprocessManager->addLogText("\n\nWARNING: The working folder has not been deleted for debugging purposes: " + tempDirPath, true);
	}
	else
	{
		if (!deleteDirectory(tempDirPath))
		{
			m_subprocessManager->addLogText("ERROR: Unable to remove the temporary working directory: " + tempDirPath, true);
		}
	}

	std::string logFileText;
	m_subprocessManager->endLogging(logFileText);
	m_subprocessManager->shutdownSubprocess();

	// Store the log text in a result item
	EntityResultText* text = this->getModelComponent()->addResultTextEntity(solver.getEntityName() + "/Output", logFileText);

	getModelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	delete text;
	text = nullptr;

	// Store the newly created items in the data base
	this->getModelComponent()->storeNewEntities("added solver results");
}


void Application::deleteSingleSolverResults(EntityBase* solverEntity)
{
	std::list<std::string> entityNameList;
	entityNameList.push_back(solverEntity->getName() + "/Results");
	entityNameList.push_back(solverEntity->getName() + "/Output");

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityNameList, false);
}

EntityMeshCartesian* Application::updateAndLoadMeshEntity(const std::string &meshName, std::list<ot::EntityInformation>& meshInfo)
{
	ot::UID meshEntityID = 0;
	ot::UID meshEntityVersion = 0;

	for (auto meshItem : meshInfo)
	{
		if (meshItem.getEntityName() == meshName)
		{
			meshEntityID = meshItem.getEntityID();
			meshEntityVersion = meshItem.getEntityVersion();

			break;
		}
	}

	if (meshEntityID == 0)
	{
		this->getUiComponent()->displayMessage("ERROR: The specified mesh does not exist: " + meshName + "\n");
		return nullptr;
	}

	// Load the mesh entity
	EntityBase* meshEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(meshEntityID, meshEntityVersion);

	if (dynamic_cast<EntityMeshCartesian*>(meshEntity) == nullptr)
	{
		delete meshEntity;
		meshEntity = nullptr;

		this->getUiComponent()->displayMessage("ERROR: The specified mesh is not a valid cartesian mesh: " + meshName + "\n");
		return nullptr;
	}

	delete meshEntity;
	meshEntity = nullptr;

	// Now update the mesh
	getModelComponent()->updateCartesianMesh(meshName);

	// And finally load the updated mesh
	ot::EntityInformation entityInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(meshName, entityInfo))
	{
		this->getUiComponent()->displayMessage("ERROR: The specified mesh is not a valid cartesian mesh: " + meshName + "\n");
		return nullptr;
	}

	meshEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());

	if (dynamic_cast<EntityMeshCartesian*>(meshEntity) == nullptr)
	{
		delete meshEntity;
		meshEntity = nullptr;

		this->getUiComponent()->displayMessage("ERROR: The specified mesh is not a valid cartesian mesh: " + meshName + "\n");
		return nullptr;
	}

	return dynamic_cast<EntityMeshCartesian*>(meshEntity);
}

std::string Application::getUniqueTempDir(void)
{
	std::string tempDir = getSystemTempDir();

	size_t count = 1;
	std::string uniqueTempDir;

	do
	{
		uniqueTempDir = tempDir + "\\OPENEMS_WORK" + std::to_string(count);
		count++;

	} while (checkFileOrDirExists(uniqueTempDir));

	return uniqueTempDir;
}

std::string Application::getSystemTempDir(void)
{
	return readEnvironmentVariable("TMP");
}

std::string Application::readEnvironmentVariable(const std::string& variableName)
{
	std::string variableValue;

	const int nSize = 32767;
	char* buffer = new char[nSize];

	if (GetEnvironmentVariableA(variableName.c_str(), buffer, nSize))
	{
		variableValue = buffer;
	}

	return variableValue;
}

bool Application::checkFileOrDirExists(const std::string& path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;   // This is a directoy
	else
		return true;   // This might be a file
}

bool Application::deleteDirectory(const std::string& pathName)
{
	// First delete all files in the directoy
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string fileNamePattern = pathName + "\\*";

	hFind = FindFirstFileA(fileNamePattern.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		std::string fileName = FindFileData.cFileName;
		if (fileName != "." && fileName != "..")
		{
			fileName = pathName + "\\" + fileName;
			if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
		}

		while (FindNextFileA(hFind, &FindFileData))
		{
			std::string fileName = FindFileData.cFileName;
			if (fileName != "." && fileName != "..")
			{
				fileName = pathName + "\\" + fileName;
				if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
			}
		}

		FindClose(hFind);
	}

	// Now remove the empty directory
	bool success = RemoveDirectoryA(pathName.c_str());
	return success;
}

std::string Application::getOpenEMSDir()
{
#ifdef _DEBUG
	const std::string otRootFolder = ot::OperatingSystem::getEnvironmentVariableString("OPENTWIN_DEV_ROOT");
	assert(otRootFolder != "");
	return otRootFolder + "\\Deployment\\openEMSSolver";
#else
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);

	std::string fullPath(path);

	size_t pos = fullPath.find_last_of("\\/");
	std::string directory = fullPath.substr(0, pos);

	return directory + "\\openEMSSolver";
#endif // DEBUG
}

