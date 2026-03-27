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
#include "OTGui/Graphics/GraphicsItemCfg.h"
#include "OTGui/Graphics/GraphicsItemCfgFactory.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UILockWrapper.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTBlockEntities/BlockHelper.h"
#include "DataLakeAccessor.h"

#include "OTModelEntities/EntitySolverDataProcessing.h"

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
	initiallySelectFirstChildEntityOf(ot::FolderNames::DataProcessingFolder);
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
				if(folderName.empty())
				{
					Application::instance()->getUiComponent()->displayMessage("No pipeline selected in solver: " + solver->getName() + "\n");
					continue;
				}
				auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID(folderName);
				auto allConnectionEntities = _blockEntityHandler.findAllEntityBlockConnections(folderName);
				std::map<ot::UID, ot::UIDList> connectionBlockMap = BlockHelper::buildMap(allConnectionEntities, allBlockEntities);
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
#include "OTModelEntities/EntityResult1DCurve.h"

std::string Application::updateDataLakeAccessCfg(ot::JsonDocument& _document)
{
	try
	{
		DataLakeQueryCfg queryCfg;
		queryCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
		DataLakeAccessor dataLakeAccessor;
		dataLakeAccessor.accessPartition(queryCfg.getCollectionName());
		dataLakeAccessor.createQueryDescriptionQuantity(queryCfg.getValueDescriptionQuantities());
		dataLakeAccessor.createQueryDescriptionsParameter(queryCfg.getValueDescriptionParameters());
		dataLakeAccessor.createQueryDescriptionsSeries(queryCfg.getValueDescriptionSeriesMD(),queryCfg.getSeriesLabel());
		ot::DataLakeAccessCfg accessConfig = dataLakeAccessor.createConfig();
	
		if (!accessConfig.getQueriesByCollection().empty())
		{
			ot::UID curveEntityID =	ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
			std::list<ot::EntityInformation> entityInfos;
			ot::ModelServiceAPI::getEntityInformation({ curveEntityID }, entityInfos);
			
			assert(entityInfos.size() == 1);
			ot::UID curveEntityVersion = entityInfos.front().getEntityVersion();
			EntityBase* base = ot::EntityAPI::readEntityFromEntityIDandVersion(curveEntityID, curveEntityVersion);
			std::unique_ptr< EntityResult1DCurve> curve(dynamic_cast<EntityResult1DCurve*>(base));
			curve->setDataLakeAccessCfg(std::move(accessConfig));
			curve->storeToDataBase();
			ot::NewModelStateInfo updatedCurve;
			updatedCurve.addTopologyEntity(*curve.get());
			ot::ModelServiceAPI::updateTopologyEntities(updatedCurve, "Performed update on the data lake access cfg.", false);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateCurvesOfPlot, doc.GetAllocator()), doc.GetAllocator());

			const std::string plotName = ot::EntityName::getParentPath(curve->getName());
			doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, doc.GetAllocator()), doc.GetAllocator());

			ot::VisualisationCfg visualisationCfg;
			visualisationCfg.setVisualisationType(OT_ACTION_CMD_VIEW1D_Setup);
			visualisationCfg.setOverrideViewerContent(false);
			visualisationCfg.setAsActiveView(true);

			doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, ot::JsonObject(visualisationCfg, doc.GetAllocator()), doc.GetAllocator());
			
			bool success = sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);
			if (!success)
			{
				return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to create data lake access config").toJson();
			}
		}
		return ot::ReturnMessage().toJson();	

	}
	catch (std::exception& _e)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed,_e.what()).toJson();
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
	
	m_buttonGraphicsScene = ot::ToolBarButtonCfg(pageName, groupName, "Create Pipeline", "Default/Add");
	_ui->addMenuButton(m_buttonGraphicsScene.setButtonLockFlags(modelWrite));

	m_buttonCreateSolver = ot::ToolBarButtonCfg(pageName, groupName, "Add Solver", "Default/AddSolver");
	_ui->addMenuButton(m_buttonCreateSolver.setButtonLockFlags(modelWrite));

	m_buttonRunPipeline = ot::ToolBarButtonCfg(pageName, groupName, "Run", "Default/RunSolver");
	_ui->addMenuButton(m_buttonRunPipeline.setButtonLockFlags(modelWrite));

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
