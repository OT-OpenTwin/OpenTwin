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

#include "ClassFactory.h"
#include "ExternalDependencies.h"
#include "ClassFactoryBlock.h"
#include "EntitySolverDataProcessing.h"


#include "OTServiceFoundation/UILockWrapper.h"
#include "EntitySolverDataProcessing.h"

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
{
	ClassFactory& classFactory = getClassFactory();
	ClassFactoryBlock* classFactoryBlock = new ClassFactoryBlock();
	classFactoryBlock->SetChainRoot(&classFactory);
	classFactory.SetNextHandler(classFactoryBlock);
}

Application::~Application()
{

}

void Application::runPipeline(ot::UIDList _selectedSolverIDs)
{
 	//UILockWrapper lockWrapper(Application::instance()->getUiComponent(), ot::LockModelWrite);
	try
	{
		Application::instance()->prefetchDocumentsFromStorage(_selectedSolverIDs);
		ClassFactory& classFactory = Application::instance()->getClassFactory();
		
		for (ot::UID entityID : _selectedSolverIDs)
		{
			ot::UID entityVersion =	Application::instance()->getPrefetchedEntityVersion(entityID);
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion, classFactory);
			std::unique_ptr<EntitySolverDataProcessing> solver (dynamic_cast<EntitySolverDataProcessing*>(baseEntity));
			const std::string folderName = solver->getName();
			auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID(folderName);
			const bool isValid = _graphHandler.blockDiagramIsValid(allBlockEntities);

			if (isValid)
			{
				const std::list<std::shared_ptr<GraphNode>>& rootNodes = _graphHandler.getRootNodes();
				const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID = _graphHandler.getgraphNodesByBlockID();
				_pipelineHandler.RunAll(rootNodes, graphNodesByBlockID, allBlockEntities);
			}
		}
	}
	catch (const std::exception& e)
	{
		OT_LOG_E("Pipeline run failed: " + std::string(e.what()));
	}
}

// ##################################################################################################################################################################################################################

// Custom functions



// ##################################################################################################################################################################################################################

// Required functions

std::string Application::processAction(const std::string& _action, ot::JsonDocument& _doc)
{
	try
	{
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == m_buttonRunPipeline.GetFullDescription())
			{
				EntitySolverDataProcessing solver(0, nullptr, nullptr, nullptr, nullptr, "");
				ot::UIDList selectedSolverIDs;
				for (ot::EntityInformation& selectedEntity : m_selectedEntityInfos)
				{
					if (selectedEntity.getEntityType() == solver.getClassName())
					{
						selectedSolverIDs.push_back(selectedEntity.getEntityID());
					}
				}
				if (selectedSolverIDs.size() == 0)
				{
					Application::instance()->uiComponent()->displayMessage("No solver selected to run.");
				}
				else
				{
					std::thread worker(&Application::runPipeline, this, selectedSolverIDs);
					worker.detach();
				}
			}
			else if (action == m_buttonCreatePipeline.GetFullDescription())
			{
				auto modelComponent = Application::instance()->getModelComponent();
				EntitySolverDataProcessing newDataprocessing(modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, Application::instance()->getServiceName());
				auto allPipelines =	ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DataProcessingFolder);
				const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::DataProcessingFolder, "Pipeline", allPipelines);
				newDataprocessing.setName(entityName);
				newDataprocessing.StoreToDataBase();
				ot::NewModelStateInformation infos;
				infos.m_topologyEntityIDs.push_back(newDataprocessing.getEntityID());
				infos.m_topologyEntityVersions.push_back(newDataprocessing.getEntityStorageVersion());
				ot::ModelServiceAPI::addEntitiesToModel(infos,"Added pipeline");
			}
		}
		else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem)
		{
			std::string itemName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
			std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

			ot::Point2DD position;
			position.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

			//Needs to be set once but modelConnect event cannot be used currently, since the modelstate in that point in time is a dummy.
			ExternalDependencies dependencies;
			if (dependencies.getPythonScriptFolderID() == 0)
			{
				ot::EntityInformation entityInfo;
				ot::ModelServiceAPI::getEntityInformation("Scripts", entityInfo);
				ExternalDependencies dependencies;
				dependencies.setPythonScriptFolderID(entityInfo.getEntityID());
			}

			_blockEntityHandler.CreateBlockEntity(editorName, itemName, position);
		}
		else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection)
		{
			ot::GraphicsConnectionPackage pckg;
			pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));
			const std::string editorName = pckg.name();

			_blockEntityHandler.AddBlockConnection(pckg.connections(), editorName);
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
		}
		else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged)
		{
			std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
			ot::GraphicsItemCfg* itemConfig = ot::GraphicsItemCfgFactory::create(ot::json::getObject(_doc, OT_ACTION_PARAM_Config));
			if (!itemConfig) return "";

			const ot::UID blockID = itemConfig->getUid();
			_blockEntityHandler.UpdateBlockPosition(blockID, itemConfig->getPosition(), &getClassFactory());

		}
	}
	catch (const std::exception& e)
	{
		OT_LOG_E(e.what());
	}
	return ""; // Return empty string if the request does not expect a return
}

void Application::propertyChanged(ot::JsonDocument& _doc)
{
	EntityBlockDatabaseAccess dbA(0, nullptr, nullptr, nullptr, nullptr, "");
	if (this->getSelectedEntities().size() == 1 && this->getSelectedEntityInfos().begin()->getEntityType() == dbA.getClassName())
	{
		auto entBase = ot::EntityAPI::readEntityFromEntityIDandVersion(this->getSelectedEntityInfos().begin()->getEntityID(), this->getSelectedEntityInfos().begin()->getEntityVersion(), getClassFactory());
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
	m_buttonRunPipeline.SetDescription(pageName, groupName, "Run");
	m_buttonCreatePipeline.SetDescription(pageName, groupName, "Create Pipeline");
	_ui->addMenuButton(m_buttonRunPipeline, modelWrite, "RunSolver");
	_ui->addMenuButton(m_buttonCreatePipeline, modelWrite, "AddSolver");
	_blockEntityHandler.setUIComponent(_ui);
	_blockEntityHandler.OrderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	_graphHandler.setUIComponent(_ui);
	_pipelineHandler.setUIComponent(_ui);
	m_propertyHandlerDatabaseAccessBlock.setUIComponent(_ui);
}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntityHandler.setModelComponent(_model);
	_graphHandler.setModelComponent(_model);
	_pipelineHandler.setModelComponent(_model);
	m_propertyHandlerDatabaseAccessBlock.setModelComponent(_model);
}
