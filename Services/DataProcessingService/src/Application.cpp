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

#include "EntitySolverDataProcessing.h"

#include "OTServiceFoundation/UILockWrapper.h"

#include "OTServiceFoundation/ProgressUpdater.h"


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
	
}

Application::~Application()
{

}


void Application::runPipeline()
{
	ot::UILockWrapper lockWrapper(Application::instance()->getUiComponent(), ot::LockType::ModelWrite);
	
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
		ProgressUpdater progressUpdater(Application::instance()->getUiComponent(), "Running Data Processing Pipeline", true);

		try
		{
			Application::instance()->prefetchDocumentsFromStorage(selectedSolverIDs);

			for (ot::UID entityID : selectedSolverIDs)
			{
				ot::UID entityVersion = Application::instance()->getPrefetchedEntityVersion(entityID);
				EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion);
				std::unique_ptr<EntitySolverDataProcessing> solver(dynamic_cast<EntitySolverDataProcessing*>(baseEntity));
				const std::string folderName = solver->getSelectedPipeline();
				auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID(folderName);
				auto allConnectionEntities = _blockEntityHandler.findAllEntityBlockConnections(folderName);
				std::map<ot::UID, ot::UIDList> connectionBlockMap = Helper::buildMap(allConnectionEntities, allBlockEntities);
				const bool isValid = _graphHandler.blockDiagramIsValid(allConnectionEntities, allBlockEntities, connectionBlockMap);

				if (isValid)
				{
					const std::list<std::shared_ptr<GraphNode>>& rootNodes = _graphHandler.getRootNodes();
					const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID = _graphHandler.getgraphNodesByBlockID();
					Application::instance()->getUiComponent()->displayMessage("\nRunning pipeline: " + solver->getName() + "\n");
					_pipelineHandler.setSolverName(solver->getName());
					_pipelineHandler.runAll(rootNodes, graphNodesByBlockID, allBlockEntities);
				}
			}
		}
		catch (const std::exception& e)
		{
			OT_LOG_E("Pipeline run failed: " + std::string(e.what()));
		}
	}
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
	ot::LockTypes modelWrite(ot::LockType::ModelWrite);

	_ui->addMenuPage(pageName);
	_ui->addMenuGroup(pageName, groupName);
	
	m_buttonRunPipeline = ot::ToolBarButtonCfg(pageName, groupName, "Run", "Default/RunSolver");
	_ui->addMenuButton(m_buttonRunPipeline.setButtonLockFlags(modelWrite));

	m_buttonCreateSolver = ot::ToolBarButtonCfg(pageName, groupName, "Add Solver", "Default/AddSolver");
	_ui->addMenuButton(m_buttonCreateSolver.setButtonLockFlags(modelWrite));

	m_buttonGraphicsScene = ot::ToolBarButtonCfg(pageName, groupName, "Create Pipeline", "Default/AddSolver");
	_ui->addMenuButton(m_buttonGraphicsScene.setButtonLockFlags(modelWrite));

	m_buttonCreateManifest = ot::ToolBarButtonCfg(pageName, groupName, "Create Manifest", "Default/AddMaterial");
	_ui->addMenuButton(m_buttonCreateManifest.setButtonLockFlags(modelWrite));


	_blockEntityHandler.setUIComponent(_ui);
	_blockEntityHandler.createBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	_graphHandler.setUIComponent(_ui);
	_pipelineHandler.setUIComponent(_ui);
	m_propertyHandlerDatabaseAccessBlock.setUIComponent(_ui);
	
	connectToolBarButton(m_buttonGraphicsScene, &m_entityCreator, &EntityCreator::createPipeline);
	connectToolBarButton(m_buttonCreateSolver, &m_entityCreator, &EntityCreator::createSolver);
	connectToolBarButton(m_buttonCreateManifest, &m_entityCreator, &EntityCreator::createManifests);
	connectToolBarButton(m_buttonRunPipeline, [this]() 
		{
			std::thread worker(&Application::runPipeline, this);
			worker.detach();
		}
	);
}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntityHandler.setModelComponent(_model);
	_graphHandler.setModelComponent(_model);
	_pipelineHandler.setModelComponent(_model);
	m_propertyHandlerDatabaseAccessBlock.setModelComponent(_model);
}
