/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"


// Open twin header
#include "OTCore/EntityName.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "TemplateDefaultManager.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "Helper.h"

#include "ExternalDependencies.h"
#include "EntitySolverDataProcessing.h"

#include "OTServiceFoundation/UILockWrapper.h"
#include "EntitySolverDataProcessing.h"
#include "EntityGraphicsScene.h"


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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService, new UiNotifier(), new ModelNotifier())
{}

Application::~Application()
{

}

void Application::runPipelineWorker(ot::UIDList _selectedSolverIDs)
{
 	//UILockWrapper lockWrapper(Application::instance()->getUiComponent(), ot::LockModelWrite);
	try
	{
		Application::instance()->prefetchDocumentsFromStorage(_selectedSolverIDs);

		for (ot::UID entityID : _selectedSolverIDs)
		{
			ot::UID entityVersion =	Application::instance()->getPrefetchedEntityVersion(entityID);
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion);
			std::unique_ptr<EntitySolverDataProcessing> solver (dynamic_cast<EntitySolverDataProcessing*>(baseEntity));
			const std::string folderName = solver->getSelectedPipeline();
			auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID(folderName);
			auto allConnectionEntities = _blockEntityHandler.findAllEntityBlockConnections(folderName);
			std::map<ot::UID, ot::UIDList> connectionBlockMap = Helper::buildMap(allConnectionEntities, allBlockEntities);
			const bool isValid = _graphHandler.blockDiagramIsValid(allConnectionEntities,allBlockEntities,connectionBlockMap);

			if (isValid)
			{
				const std::list<std::shared_ptr<GraphNode>>& rootNodes = _graphHandler.getRootNodes();
				const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID = _graphHandler.getgraphNodesByBlockID();
				Application::instance()->getUiComponent()->displayMessage("\nRunning pipeline: " + solver->getName() + "\n");
				_pipelineHandler.setSolverName(solver->getName());
				_pipelineHandler.RunAll(rootNodes, graphNodesByBlockID, allBlockEntities);
			}
		}
	}
	catch (const std::exception& e)
	{
		OT_LOG_E("Pipeline run failed: " + std::string(e.what()));
	}
}

void Application::createPipeline()
{
	auto modelComponent = Application::instance()->getModelComponent();
	EntityGraphicsScene newDataprocessing(modelComponent->createEntityUID(), nullptr, nullptr, nullptr, Application::instance()->getServiceName());

	auto allPipelines = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DataProcessingFolder);
	const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::DataProcessingFolder, "Pipeline", allPipelines);
	newDataprocessing.setName(entityName);
	newDataprocessing.setEditable(true);
	newDataprocessing.storeToDataBase();

	ot::NewModelStateInfo infos;
	infos.addTopologyEntity(newDataprocessing);
	ot::ModelServiceAPI::addEntitiesToModel(infos, "Added pipeline");
}

void Application::createSolver()
{
	auto modelComponent = Application::instance()->getModelComponent();
	EntitySolverDataProcessing newSolver(modelComponent->createEntityUID(), nullptr, nullptr, nullptr, Application::instance()->getServiceName());

	if (m_dataProcessingFolderID == ot::getInvalidUID())
	{
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(ot::FolderNames::DataProcessingFolder, entityInfo);
		m_dataProcessingFolderID = entityInfo.getEntityID();
	}
	newSolver.createProperties(ot::FolderNames::DataProcessingFolder, m_dataProcessingFolderID);

	auto allPipelines = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::SolverFolder);
	const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::SolverFolder, "Pipeline Solver", allPipelines);
	newSolver.setName(entityName);

	newSolver.storeToDataBase();
	ot::NewModelStateInfo entityInfos;
	entityInfos.addTopologyEntity(newSolver);
	ot::ModelServiceAPI::addEntitiesToModel(entityInfos, "Added solver");

}

void Application::runPipeline()
{
	EntitySolverDataProcessing solver;
	ot::UIDList selectedSolverIDs;
	for (const ot::EntityInformation& selectedEntity : getSelectedEntityInfos())
	{
		if (selectedEntity.getEntityType() == solver.getClassName())
		{
			selectedSolverIDs.push_back(selectedEntity.getEntityID());
		}
	}
	if (selectedSolverIDs.size() == 0)
	{
		Application::instance()->getUiComponent()->displayMessage("No solver selected to run.");
	}
	else
	{
		std::thread worker(&Application::runPipelineWorker, this, selectedSolverIDs);
		worker.detach();
	}
}

#include "EntityDatabaseIndexManipulator.h"
#include "IndexHandler.h"
#include "ProjectToCollectionConverter.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "NewModelStateInfo.h"

void Application::createDefaultIndexes()
{
	const bool continuousProgressbar = true;
	ProgressUpdater updater(Application::instance()->getUiComponent(), "Creating database indexes", continuousProgressbar);

	//A single entity of this type shall be added if it does not exist yet.
	std::string projectName = "";
	ot::UIDList itemIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(ot::FolderNames::DatasetFolder, EntityDatabaseIndexManipulator::className(), false);

	if (itemIDs.size() == 0)
	{
		projectName = Application::instance()->getProjectName();

		EntityDatabaseIndexManipulator indexManipulator;
		indexManipulator.setEntityID(Application::instance()->getModelComponent()->createEntityUID());
		indexManipulator.createProperties(projectName);
		indexManipulator.setEditable(false);
		indexManipulator.storeToDataBase();
		std::list<std::string> defaultIndexes = { IndexHandler::getDefaultIndexes().begin(),IndexHandler::getDefaultIndexes().end() };
		indexManipulator.createIndexes(defaultIndexes);

		ot::NewModelStateInfo infos;
		infos.addTopologyEntity(indexManipulator);
		ot::ModelServiceAPI::addEntitiesToModel(infos, "Added database index manipulator");
	}

	const std::string sessionServiceURL = Application::instance()->getSessionServiceURL();
	auto modelComponent = Application::instance()->getModelComponent();

	ProjectToCollectionConverter collectionFinder(sessionServiceURL);
	std::string loggedInUserName = Application::instance()->getLogInUserName();
	std::string loggedInUserPsw = Application::instance()->getLogInUserPassword();
	const std::string collectionName = collectionFinder.nameCorrespondingCollection(projectName, loggedInUserName, loggedInUserPsw);


	ResultImportLogger logger;
	IndexHandler indexHandler(collectionName, logger);
	indexHandler.createDefaultIndexes();
}

// ##################################################################################################################################################################################################################

// Protected: Callback functions

ot::ReturnMessage Application::graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) {
	ExternalDependencies dependencies;
	if (dependencies.getPythonScriptFolderID() == 0) {
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(ot::FolderNames::PythonScriptFolder, entityInfo);
		ExternalDependencies dependencies;
		dependencies.setPythonScriptFolderID(entityInfo.getEntityID());
	}

	_blockEntityHandler.createBlockEntity(_viewName, _itemName, _pos);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) {
	_blockEntityHandler.addBlockConnection(_connectionData.connections(), _connectionData.name());

	//EntitySolverDataProcessing solver(0, nullptr, nullptr, nullptr, nullptr, "");
	//std::list<std::string> selectedSolverName;
	//for (ot::EntityInformation& selectedEntity : m_selectedEntityInfos)
	//{
	//	if (selectedEntity.getEntityType() == solver.getClassName())
	//	{
	//		selectedSolverName.push_back(selectedEntity.getEntityName());
	//	}
	//}

	//if (selectedSolverName.size() == 1)
	//{
	//}
	//else
	//{
	//	assert(0); //A connection should be drawn in only one graphics view.
	//}

	return ot::ReturnMessage::Ok;
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::propertyChanged(ot::JsonDocument& _doc)
{
	EntityBlockDatabaseAccess dbA;
	std::list<ot::EntityInformation> dbAccessBlockInformations;
	const std::list<ot::EntityInformation> selectedEntitiesInfos =	this->getSelectedEntityInfos();
	for (const ot::EntityInformation& selectedEntityInfo :selectedEntitiesInfos )
	{
		if (selectedEntityInfo.getEntityType() == dbA.getClassName())
		{
			dbAccessBlockInformations.push_back(selectedEntityInfo);
		}
	}
	
	Application::instance()->prefetchDocumentsFromStorage(dbAccessBlockInformations);
	for(ot::EntityInformation& selectedEntityInfos : dbAccessBlockInformations)
	{
		auto entBase = ot::EntityAPI::readEntityFromEntityIDandVersion(selectedEntityInfos.getEntityID(), selectedEntityInfos.getEntityVersion());
		auto dbAccess = std::shared_ptr<EntityBlockDatabaseAccess>(dynamic_cast<EntityBlockDatabaseAccess*>(entBase));
		if (dbAccess != nullptr)
		{
			m_propertyHandlerDatabaseAccessBlock.performEntityUpdateIfRequired(dbAccess);
		}
		else
		{
			assert(false);
		}
	}
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	const std::string pageName = "Data Processing";
	const std::string groupName = "Pipeline Handling";
	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuPage(pageName);
	_ui->addMenuGroup(pageName, groupName);
	
	m_buttonRunPipeline = ot::ToolBarButtonCfg(pageName, groupName, "Run", "Default/RunSolver");
	_ui->addMenuButton(m_buttonRunPipeline.setButtonLockFlags(modelWrite));

	m_buttonCreateSolver = ot::ToolBarButtonCfg(pageName, groupName, "Add Solver", "Default/AddSolver");
	_ui->addMenuButton(m_buttonCreateSolver.setButtonLockFlags(modelWrite));

	m_buttonGraphicsScene = ot::ToolBarButtonCfg(pageName, groupName, "Create Pipeline", "Default/AddSolver");
	_ui->addMenuButton(m_buttonGraphicsScene.setButtonLockFlags(modelWrite));

	m_buttonDefaultIndexCreation = ot::ToolBarButtonCfg(pageName, "Database Indexes", "Update Indexes", "Default/IndexCreation");
	_ui->addMenuButton(m_buttonDefaultIndexCreation.setButtonLockFlags(modelWrite));

	_blockEntityHandler.setUIComponent(_ui);
	_blockEntityHandler.orderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	_graphHandler.setUIComponent(_ui);
	_pipelineHandler.setUIComponent(_ui);
	m_propertyHandlerDatabaseAccessBlock.setUIComponent(_ui);
	
	connectToolBarButton(m_buttonGraphicsScene, this, &Application::createPipeline);
	connectToolBarButton(m_buttonCreateSolver, this, &Application::createSolver);
	connectToolBarButton(m_buttonRunPipeline, this, &Application::runPipeline);

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntityHandler.setModelComponent(_model);
	_graphHandler.setModelComponent(_model);
	_pipelineHandler.setModelComponent(_model);
	m_propertyHandlerDatabaseAccessBlock.setModelComponent(_model);
}
