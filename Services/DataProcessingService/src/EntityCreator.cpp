// @otlicense
// File: EntityCreator.cpp
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

// OpenTwin header
#include "EntityCreator.h"
#include "EntityPythonManifest.h"
#include "Application.h"
#include "OTCore/EntityName.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "NewModelStateInfo.h"
#include "OTCore/FolderNames.h"
#include "EntityGraphicsScene.h"
#include "EntitySolverDataProcessing.h"

void EntityCreator::createManifests()
{
	EntityPythonManifest newManifest;
	auto modelComponent = Application::instance()->getModelComponent();
	newManifest.setEntityID(modelComponent->createEntityUID());
	const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::PythonManifestFolder, "Environment", ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::PythonManifestFolder));
	newManifest.setName(entityName);
	newManifest.registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection,
		Application::instance()->getServiceName()
	);

	newManifest.storeToDataBase();
	ot::NewModelStateInfo newModelState;
	newModelState.addTopologyEntity(newManifest);
	ot::ModelServiceAPI::addEntitiesToModel(newModelState, "Added Python Manifest");
}


void EntityCreator::createPipeline()
{
	auto modelComponent = Application::instance()->getModelComponent();
	EntityGraphicsScene newDataprocessing(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	newDataprocessing.setGraphicsPickerKey(OT_INFO_SERVICE_TYPE_DataProcessingService);
	newDataprocessing.registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection,
		Application::instance()->getServiceName()
	);

	auto allPipelines = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DataProcessingFolder);
	const std::string entityName = ot::EntityName::createUniqueEntityName(ot::FolderNames::DataProcessingFolder, "Pipeline", allPipelines);
	newDataprocessing.setName(entityName);
	newDataprocessing.setTreeItemEditable(true);

	if(m_manifestFolderID == ot::getInvalidUID())
	{
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(ot::FolderNames::PythonManifestFolder, entityInfo);
		m_manifestFolderID = entityInfo.getEntityID();
	}
	
	newDataprocessing.storeToDataBase();

	ot::NewModelStateInfo infos;
	infos.addTopologyEntity(newDataprocessing);
	ot::ModelServiceAPI::addEntitiesToModel(infos, "Added pipeline");
}

void EntityCreator::createSolver()
{
	auto modelComponent = Application::instance()->getModelComponent();
	EntitySolverDataProcessing newSolver(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
	newSolver.registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection,
		Application::instance()->getServiceName()
	);

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
