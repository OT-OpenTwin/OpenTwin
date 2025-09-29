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

#include "OTServiceFoundation/UILockWrapper.h"

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

void Application::runPipeline()
{
 	//UILockWrapper lockWrapper(Application::instance()->getUiComponent(), ot::LockModelWrite);
	try
	{

		auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID();
		const bool isValid = _graphHandler.blockDiagramIsValid(allBlockEntities);
		if (isValid)
		{
			const std::list<std::shared_ptr<GraphNode>>& rootNodes = _graphHandler.getRootNodes();
			const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID = _graphHandler.getgraphNodesByBlockID();
			_pipelineHandler.RunAll(rootNodes, graphNodesByBlockID, allBlockEntities);
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
			if (action == _buttonRunPipeline.GetFullDescription())
			{
				std::thread worker(&Application::runPipeline, this);
				worker.detach();
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
	_buttonRunPipeline.SetDescription(pageName, groupName, "Run");
	_ui->addMenuButton(_buttonRunPipeline, modelWrite, "RunSolver");
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
