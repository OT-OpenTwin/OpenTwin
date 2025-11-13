// @otlicense
// File: Model.cpp
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

// Model.cpp : Defines the Model class which is exported for the DLL application.
//

#include "stdafx.h"
#include "Model.h"
#include "Application.h"
#include "EntityBase.h"
#include "EntityUnits.h"
#include "EntityMeshTet.h"
#include "EntityVis2D3D.h"
#include "EntityMaterial.h"
#include "EntityGeometry.h"
#include "EntityParameter.h"
#include "EntityAnnotation.h"
#include "EntityMeshTetItem.h"
#include "EntityGraphicsScene.h"
#include "EntityMeshCartesian.h"
#include "EntityFaceAnnotation.h"
#include "EntityMetadataCampaign.h"
#include "EntityParameterizedDataPreviewTable.h"
#include "EntityParameterizedDataCategorization.h"
#include "EntityBlockConnection.h"
#include "OTGui/SelectEntitiesDialogCfg.h"
#include "OTGuiAPI/Frontend.h"

#include "MicroserviceNotifier.h"
#include "GeometryOperations.h"
#include "TemplateDefaultManager.h"
#include "TableReader.h"
#include "ProgressReport.h"
#include "ProjectTypeManager.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"
#include "OTCommunication/ActionTypes.h"

#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepBndLib.hxx"
#include "Bnd_Box.hxx"
#include "BRepExtrema_DistShapeShape.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"

#undef min
#undef max

#include "DataBase.h"
#include "Document\DocumentAccess.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>

#include <thread>	
#include <iostream>
#include <set>
#include <memory>

#include "tinyexpr.h"

// OpenTwin header
#include "EntityBlock.h"
#include "OTCore/String.h"
#include "OTGui/KeySequence.h"
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyString.h"
#include "OTGui/VersionGraphVersionCfg.h"

#include "MetadataEntityInterface.h"

#include "OTServiceFoundation/UILockWrapper.h"
#include "OTCore/FolderNames.h"

// Observer
void Model::entityRemoved(EntityBase *entity) 
{ 
	removeEntityFromMap(entity, true, true); 
}

void Model::entityModified(EntityBase *entity) 
{ 
	setModified(); 
}

// Model class

Model::Model(const std::string &_projectName, const std::string& _projectType, const std::string &_collectionName) :
	entityRoot(nullptr),
	modelStorageVersion(0),
	anyDataChangeSinceLastWrite(true),
	visualizationModelID(0),
	isModified(true),
	projectName(_projectName),
	projectType(_projectType),
	collectionName(_collectionName),
	shutdown(false),
	uiCreated(false),
	versionGraphCreated(false),
	stateManager(nullptr),
	m_isProjectOpen(false)
{
	//NOTE, debug only
	std::cout << "Created model for project \"" << _projectName << "\"" << std::endl;
	
	m_infoButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Geometry", "Info", "Default/Information");
	m_infoButton.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::I));
	m_infoButton.setButtonLockFlags(ot::LockType::ModelRead);
	m_buttonHandler.connectToolBarButton(m_infoButton, this, &Model::handleShowSelectedShapeInformation);

	m_createParameterButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Parameters", "Create Parameter", "Default/CreateParameter");
	m_createParameterButton.setButtonLockFlags(ot::LockType::ModelRead);
	m_buttonHandler.connectToolBarButton(m_createParameterButton, this, &Model::handleCreateNewParameter);

	m_undoButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Edit", "Undo", "Default/Undo");
	m_undoButton.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::Z));
	m_undoButton.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonHandler.connectToolBarButton(m_undoButton, this, &Model::handleUndoLastOperation);

	m_redoButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Edit", "Redo", "Default/Redo");
	m_redoButton.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::Y));
	m_redoButton.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonHandler.connectToolBarButton(m_redoButton, this, &Model::handleRedoNextOperation);

	m_deleteButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Edit", "Delete", "Default/Delete");
	m_deleteButton.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Delete));
	m_deleteButton.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonHandler.connectToolBarButton(m_deleteButton, this, &Model::handleDeleteSelectedShapes);
	
	// Create a new project structure
	resetToNew();

	// Create the UI ribbon and actions
	auto ui = Application::instance()->getUiComponent();
	if (ui)
	{
		enableQueuingHttpRequests(true);
		setupUIControls(ui);
		enableQueuingHttpRequests(false);
	}

	// Here we set a dummy project name (needs to be changed later on)
	DataBase::instance().setCollectionName(collectionName);
}

void Model::clearAll()
{
	if (visualizationModelID != 0)
	{
		std::list<ot::UID> modelEntityIDs;

		for (const auto& entity : entityMap)
		{
			if (entity.second != nullptr) {
				modelEntityIDs.push_back(entity.first);
			}
		}

		removeShapesFromVisualization(modelEntityIDs);
	}

	modelStorageVersion = 0;

	// Reset the model state
	getStateManager()->reset();

	// Reset all the temporary attributes
	entityMap.clear();
	pendingEntityUpdates.clear();
	Application::instance()->getSelectionHandler().clearAllBuffer();
	parameterMap.clear();

	// Now we delete all entities (recursively)
	if (entityRoot != nullptr) delete entityRoot;
	entityRoot = nullptr;

	// We have changed the model data
	anyDataChangeSinceLastWrite = true;

	// Set the modification flat
	isModified = true;
}

void Model::resetToNew()
{
	clearAll();

	ProjectTypeManager typeManager(projectType);

	entityRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
	entityMap[entityRoot->getEntityID()] = entityRoot;

	GeometryOperations::EntityList allNewEntities;

	// Create the various root items
	if (typeManager.hasGeometryRoot())
	{
		EntityBase* entityGeometryRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityGeometryRoot->setName(getGeometryRootName());
		addEntityToModel(entityGeometryRoot->getName(), entityGeometryRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasCircuitsRoot())
	{
		EntityBase* entityCircuitsRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityCircuitsRoot->setName(getCircuitsRootName());
		addEntityToModel(entityCircuitsRoot->getName(), entityCircuitsRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasCircuit())
	{
		EntityGraphicsScene* entityCircuit = new EntityGraphicsScene(createEntityUID(), nullptr, nullptr, nullptr);
		entityCircuit->setName(typeManager.getCircuitName());
		entityCircuit->setGraphicsPickerKey(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		entityCircuit->registerCallbacks(
			ot::EntityCallbackBase::Callback::Properties |
			ot::EntityCallbackBase::Callback::Selection |
			ot::EntityCallbackBase::Callback::DataNotify,
			OT_INFO_SERVICE_TYPE_CircuitSimulatorService
		);
		addEntityToModel(entityCircuit->getName(), entityCircuit, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasMaterialRoot())
	{
		EntityBase* entityMaterialRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityMaterialRoot->setName(getMaterialRootName());
		addEntityToModel(entityMaterialRoot->getName(), entityMaterialRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasMeshRoot())
	{
		EntityBase* entityMeshRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityMeshRoot->setName(getMeshRootName());
		addEntityToModel(entityMeshRoot->getName(), entityMeshRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasSolverRoot())
	{
		EntityContainer* entitySolverRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entitySolverRoot->setName(getSolverRootName());
		addEntityToModel(entitySolverRoot->getName(), entitySolverRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasScriptsRoot())
	{
		EntityContainer* entityScriptRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityScriptRoot->setName(getScriptsRootName());
		addEntityToModel(entityScriptRoot->getName(), entityScriptRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasDataProcessingRoot())
	{
		EntityContainer* dataProcessingRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		dataProcessingRoot->setName(ot::FolderNames::DataProcessingFolder);
		addEntityToModel(dataProcessingRoot->getName(), dataProcessingRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasUnitRoot())
	{
		auto entityUnits = new EntityUnits(createEntityUID(), nullptr, this, getStateManager());
		entityUnits->setName(getUnitRootName());
		entityUnits->createProperties();
		//entityUnits->storeToDataBase();
		addEntityToModel(entityUnits->getName(), entityUnits, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasDataCategorizationRoot())
	{
		EntityBase* entityRMDCategorizationRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityRMDCategorizationRoot->setName(typeManager.getDataCategorizationRootName());
		addEntityToModel(entityRMDCategorizationRoot->getName(), entityRMDCategorizationRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasRMDCategorization())
	{
		auto newDataCatEntity = (new EntityParameterizedDataCategorization(createEntityUID(), nullptr, nullptr, nullptr));
		newDataCatEntity->CreateProperties(EntityParameterizedDataCategorization::DataCategorie::researchMetadata);
		newDataCatEntity->setName(typeManager.getRMDCategorizationName());
		newDataCatEntity->setEditable(false);
		addEntityToModel(newDataCatEntity->getName(), newDataCatEntity, entityRoot, true, allNewEntities);
	}

	//if (typeManager.hasRMDCategorizationPreview())
	//{
	//	auto rmdCategorizationPreview = new EntityParameterizedDataPreviewTable(createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	//	rmdCategorizationPreview->setName(typeManager.getRMDCategorizationName() + "/Preview");
	//	addEntityToModel(rmdCategorizationPreview->getName(), rmdCategorizationPreview, entityRoot, true, allNewEntities);
	//}

	if (typeManager.hasDatasetRoot())
	{
		EntityBase* entityDatasetRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityDatasetRoot->setName(typeManager.getDatasetRootName());
		addEntityToModel(entityDatasetRoot->getName(), entityDatasetRoot, entityRoot, true, allNewEntities);
	}

	if (typeManager.hasDatasetRMD())
	{
		EntityMetadataCampaign* rmd = (new EntityMetadataCampaign(createEntityUID(), nullptr, this, getStateManager()));
		rmd->setName(typeManager.getDatasetRMD());
		rmd->registerCallbacks(
			ot::EntityCallbackBase::Callback::Properties |
			ot::EntityCallbackBase::Callback::Selection |
			ot::EntityCallbackBase::Callback::DataNotify,
			OT_INFO_SERVICE_TYPE_ImportParameterizedDataService
		);
		addEntityToModel(rmd->getName(), rmd, entityRoot, true, allNewEntities);
	}
	
	if (typeManager.hasHierarchicalRoot()) {
		EntityGraphicsScene* hierarchicalRoot = new EntityGraphicsScene(createEntityUID(), nullptr, this, getStateManager());
		hierarchicalRoot->setName(ot::FolderNames::HierarchicalProjectRoot);
		hierarchicalRoot->setGraphicsPickerKey(OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		hierarchicalRoot->setSelectChildren(false);
		hierarchicalRoot->setManageChildVisibility(false);
		hierarchicalRoot->setManageParentVisibility(false);

		OldTreeIcon icons;
		icons.visibleIcon = "Default/Hierarchical";
		icons.hiddenIcon = "Default/Hierarchical";
		hierarchicalRoot->setTreeIcon(icons);

		hierarchicalRoot->registerCallbacks(
			ot::EntityCallbackBase::Callback::Properties |
			ot::EntityCallbackBase::Callback::Selection |
			ot::EntityCallbackBase::Callback::DataHandle,
			OT_INFO_SERVICE_TYPE_HierarchicalProjectService
		);
		addEntityToModel(hierarchicalRoot->getName(), hierarchicalRoot, entityRoot, true, allNewEntities);
	}

	// Now we load the default materials from the template definition
	if (!DataBase::instance().getCollectionName().empty())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
		loadDefaultMaterials();
	}

	// We have changed the model data
	anyDataChangeSinceLastWrite = true;

	// Now create the visualization items if needed
	if (Application::instance()->isUiConnected())
	{
		enableQueuingHttpRequests(true);
		createVisualizationItems();
		enableQueuingHttpRequests(false);
	}
}

Model::~Model()
{
	shutdown = true;

	// Restore the original version if needed
	// This must be called before cleaerAll since clearAll will reset the state manager
	if (stateManager) {
		stateManager->restoreOriginalVersionIfNeeded();
	}

	// delete all entities
	clearAll();

	if (stateManager != nullptr) {
		delete stateManager;
		stateManager = nullptr;
	}
}

void Model::detachAllViewer()
{
	visualizationModelID = 0;
}

void Model::setupUIControls(ot::components::UiComponent* _ui)
{	
	assert(!uiCreated);

	ot::LockTypes modelRead(ot::LockType::ModelRead);
	ot::LockTypes modelWrite(ot::LockType::ModelWrite);

	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");

	// Add default pages, groups and buttons
	_ui->addMenuPage("Model");
	_ui->addMenuGroup("Model", "Edit");
	_ui->addMenuButton(m_undoButton);
	_ui->addMenuButton(m_redoButton);
	_ui->addMenuButton(m_deleteButton);

	if (this->getProjectType() != OT_ACTION_PARAM_SESSIONTYPE_HIERARCHICAL &&
		this->getProjectType() != OT_ACTION_PARAM_SESSIONTYPE_FILEMANAGEMENT) {
		// Add pages, groups and buttons for non-hierarchical projects

		_ui->addMenuPage("View");

		//_ui->addMenuGroup("Model", "Database");
		_ui->addMenuGroup("Model", "Geometry");
		_ui->addMenuGroup("Model", "Material");
		_ui->addMenuGroup("Model", "Parameters");
		_ui->addMenuGroup("Model", "Plots");

		_ui->addMenuButton(m_infoButton);
		_ui->addMenuButton(m_createParameterButton);
	}

	Application::instance()->addButtons();

	uiCreated = true;

	// Send an initial notification to properly set the state of the new controls
	Application::instance()->getSelectionHandler().clearAllBufferAndNotify();

	updateUndoRedoStatus();
}

void Model::updateUndoRedoStatus()
{
	if (!uiCreated) return;

	enableQueuingHttpRequests(true);

	// Update the text for the undo button
	std::string text = getStateManager()->getCurrentModelStateDescription();
	if (text.empty()) text = "Undo";
	text += " (Ctrl+Z)";
	Application::instance()->getNotifier()->setToolTip("Model:Edit:Undo", text);

	// Update the text for the redo button
	text = getStateManager()->getRedoModelStateDescription();
	if (text.empty()) text = "Redo";
	text += " (Ctrl+Y)";
	Application::instance()->getNotifier()->setToolTip("Model:Edit:Redo", text);

	// Update the state for undo and redo button
	std::list<std::string> enabled;
	std::list<std::string> disabled;

	if (getStateManager()->canUndo())
	{
		enabled.push_back("Model:Edit:Undo");
	}
	else
	{
		disabled.push_back("Model:Edit:Undo");
	}

	if (getStateManager()->canRedo())
	{
		enabled.push_back("Model:Edit:Redo");
	}
	else
	{
		disabled.push_back("Model:Edit:Redo");
	}

	Application::instance()->getNotifier()->enableDisableControls(enabled, disabled);

	enableQueuingHttpRequests(false);
}

void Model::addEntityToModel(std::string entityPath, EntityBase *entity, EntityBase *root, bool addVisualizationContainers, std::list<EntityBase *> &newEntities )
{
	// This methods adds the entity with the given name to the model. If necessary, it creates container entities to build the entity tree.
	// Also, UIDs are assigned to all entities and the model pointer is set to this model.
	// All newly added entities are stored in the newEntities list

	// First of all get the first part of the entity path and check whether the folder is already present

	const char separator = '/';
	std::string::size_type pos;

	if ((pos = entityPath.find(separator)) == std::string::npos)
	{
		// The entity name does not contain a path -> add the entity to the root
		EntityContainer *container = dynamic_cast<EntityContainer *>(root);

		if (container == nullptr)
		{
			// The problem here is that the parent item exists, but is not a container. This should not happen if the model is described by a proper tree.
			// Seet a new name to the entity and add it again
			
			std::map<std::string, bool> entityNames = getListOfEntityNames();

			int count = 1;
			do
			{
				entity->setName(getGeometryRootName() + "/import_" + std::to_string(count));
			}
			while (entityNames[entity->getName()]);

			// Now we have a unique name for the entity
			addEntityToModel(entity->getName(), entity, entityRoot, addVisualizationContainers, newEntities);
			return;
		}
		else
		{
			// Add the item to the container
			if (entity->getEntityID() == 0)
			{
				// The entity Id has not yet been defined, so we need to add it here
				entity->setEntityID(createEntityUID());
			}
			entity->setObserver(this);
			entity->setModelState(getStateManager());
			entity->setParent(container);
			container->addChild(entity);

			addEntityToMap(entity);
			newEntities.push_back(entity);
			setModified();
		}
	}
	else
	{
		// Extract the first part and search for the item
		std::string folderName = entityPath.substr(0, pos);
		std::string entityName = entityPath.substr(pos + 1);

		if (root != entityRoot) folderName = root->getName() + "/" + folderName;

		// Check whetehr the container already exists and if not, create it
		EntityContainer *containerRoot = dynamic_cast<EntityContainer *>(root);
		assert(containerRoot != nullptr);

		std::list<EntityBase *> childList = containerRoot->getChildrenList();
		EntityContainer *container = nullptr;

		for (auto child : childList)
		{
			if (child->getName() == folderName)
			{
				container = dynamic_cast<EntityContainer *>(child);

				if (container == nullptr)
				{
					std::map<std::string, bool> entityNames = getListOfEntityNames();

					int count = 0;
					do
					{
						entity->setName(getGeometryRootName() + "/import_" + std::to_string(++count));
					} 
					while (entityNames[entity->getName()]);

					// Now we have a unique name for the entity
					addEntityToModel(entity->getName(), entity, entityRoot, addVisualizationContainers, newEntities);
					return;
				}
				break;
			}
		}

		if (container == nullptr)
		{
			// The container does not exist, create a new item
			container = new EntityContainer(createEntityUID(), root, this, getStateManager());

			container->setName(folderName);
			container->setEditable(entity->getEditable() && entityRoot != containerRoot); // If the entity is editable, a newly created container 
																						  // should be editable, too - if it is not a top level container
			container->setCreateVisualizationItem(addVisualizationContainers);

			containerRoot->addChild(container);

			addEntityToMap(container);
			newEntities.push_back(container);
			setModified();

			// Here we also need to add a new visualitation container item to the visualization model
			if (addVisualizationContainers)
			{
				addVisualizationContainerNode(folderName, container->getEntityID(), container->getEditable());
			}
		}

		// Now recursively add the entity
		addEntityToModel(entityName, entity, container, addVisualizationContainers, newEntities);
	}
}

void  Model::addEntityToMap(EntityBase *entity)
{
	entityMap[entity->getEntityID()] = entity;
	setModified();

	// Check whether a parameter has been added
	EntityParameter *parameter = dynamic_cast<EntityParameter *>(entity);

	if (parameter != nullptr)
	{
		setParameter(parameter->getName(), parameter->getNumericValue(), parameter);
	}
}

void Model::removeEntityWithChildrenFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, std::list<EntityBase*> &removedEntities)
{
	EntityContainer *container = dynamic_cast<EntityContainer*>(entity);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			removeEntityWithChildrenFromMap(child, keepInProject, keepParameterDependency, removedEntities);
		}
	}

	removeEntityFromMap(entity, keepInProject, keepParameterDependency);
	removedEntities.push_back(entity);
}

void  Model::removeEntityFromMap(EntityBase *entity, bool keepInProject, bool keepParameterDependency, bool considerChildren)
{
	if (!keepParameterDependency)
	{
		// Remove the dependencies on this entity
		removeParameterDependency(entity->getEntityID());
	}

	// Depending on the operation, the entity might already have been removed from the list
	entityMap.erase(entity->getEntityID());
	
	// Now we also remove the entity from the model state manager (this will also remove all children from the state Manager)
	// Removing the children here from the state manager is important, since not the entire information about an entity may be loaded 
	// (e.g. the data entities). In this case, the remove function would not automatically be called for these entities although they are
	// no longer referenced in the model.

	if (!keepInProject)
	{
		getStateManager()->removeEntity(entity->getEntityID(),considerChildren);
	}

	Application::instance()->getSelectionHandler().deselectEntity(entity->getEntityID());
	setModified();
}

bool Model::entityExists(ot::UID uID)
{
	return (entityMap.count(uID) > 0);
}

ot::UID Model::createEntityUID()
{
	return getStateManager()->createEntityUID();
}

void Model::addEntityNamesToList(EntityBase *entity, std::map<std::string, bool> &list)
{
	list[entity->getName()] = true;

	EntityContainer *entityContainer = dynamic_cast<EntityContainer *> (entity);

	if (entityContainer != nullptr)
	{
		std::list<EntityBase *> children = entityContainer->getChildrenList();

		for (auto child : children)
		{
			addEntityNamesToList(child, list);
		}
	}
}

void Model::addEntityUIToList(EntityBase *entity, std::map<ot::UID, bool> &list)
{
	list[entity->getEntityID()] = true;

	EntityContainer *entityContainer = dynamic_cast<EntityContainer *> (entity);

	if (entityContainer != nullptr)
	{
		std::list<EntityBase *> children = entityContainer->getChildrenList();

		for (auto child : children)
		{
			addEntityUIToList(child, list);
		}
	}
}

std::map<std::string, bool> Model::getListOfEntityNames()
{
	std::map<std::string, bool> existingEntityNames;

	std::list<EntityBase *> entityList = entityRoot->getChildrenList();

	for (auto entity : entityList)
	{
		addEntityNamesToList(entity, existingEntityNames);
	}

	return existingEntityNames;
}

void Model::addEntityIDsToList(EntityBase *entity, std::map<std::string, ot::UID> &list)
{
	list[entity->getName()] = entity->getEntityID();

	EntityContainer *entityContainer = dynamic_cast<EntityContainer *> (entity);

	if (entityContainer != nullptr)
	{
		std::list<EntityBase *> children = entityContainer->getChildrenList();

		for (auto child : children)
		{
			addEntityIDsToList(child, list);
		}
	}
}

std::map<std::string, ot::UID> Model::getEntityNameToIDMap()
{
	std::map<std::string, ot::UID> entityNameToIDMap;

	std::list<EntityBase *> entityList = entityRoot->getChildrenList();

	for (auto entity : entityList)
	{
		addEntityIDsToList(entity, entityNameToIDMap);
	}

	return entityNameToIDMap;
}

void Model::getModelBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	xmin = xmax = ymin = ymax = zmin = zmax = 0.0;

	entityRoot->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax);
}

void Model::setVisualizationModel(ot::UID visModelID)
{
	visualizationModelID = visModelID;	
		
	if (Application::instance()->isUiConnected())
	{
		enableQueuingHttpRequests(true);

		if (entityRoot != nullptr)
		{
			// Ensure that the root nodes have a visualization item
			//createVisualizationItems();

			entityRoot->addVisualizationNodes();
		}

		// Request a view reset
		Application::instance()->getNotifier()->resetAllViews(visualizationModelID);

		Application::instance()->getSelectionHandler().clearAllBufferAndNotify();

		updateVersionGraph();

		enableQueuingHttpRequests(false);
	}
	else
	{
		OT_LOG_EA("UI needs to be available before the visualization model is set");
	}
}

void Model::createVisualizationItems()
{
	if (visualizationModelID == 0) return;

	// Ensure that the root nodes have a visualization item
	std::list<EntityBase *> entityList = entityRoot->getChildrenList();

	for (auto entity : entityList)
	{
		auto temp = dynamic_cast<EntityContainer*>(entity);
		if (temp != nullptr && false)
		{
			addVisualizationContainerNode(entity->getName(), entity->getEntityID(), entity->getEditable());
		}
		else
		{
			entity->addVisualizationNodes();
		}
	}
}

void Model::addVisualizationContainerNode(const std::string &name, ot::UID entityID, bool isEditable)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ContainerVisible";
	treeIcons.hiddenIcon = "ContainerHidden";
	Application::instance()->getNotifier()->addVisualizationContainerNode(visualizationModelID, name, entityID, treeIcons, isEditable);
}

void Model::addVisualizationMeshNode(const std::string &name, ot::UID entityID)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "MeshVisible";
	treeIcons.hiddenIcon = "MeshHidden";
	Application::instance()->getNotifier()->addVisualizationContainerNode(visualizationModelID, name, entityID, treeIcons, false);
}

ot::UID Model::getVisualizationModel()
{
	return visualizationModelID;
}

void Model::addModalCommand(ModalCommandBase *command)
{
	assert(std::find(modalCommands.begin(), modalCommands.end(), command) == modalCommands.end()); // Ensure that the command is not yet in the list
	
	modalCommands.push_back(command);
}

void Model::removeModalCommand(ModalCommandBase *command)
{
	assert(std::find(modalCommands.begin(), modalCommands.end(), command) != modalCommands.end());  // Ensure that the command is in the list

	modalCommands.erase(std::find(modalCommands.begin(), modalCommands.end(), command));
}

void Model::requestImportTableFile(const std::string& _itemName)
{
	// Get a file name for the Table file from the UI

	newTableItemName = _itemName;
	Application::instance()->getNotifier()->requestFileForReading("Import Table File", "Table files (*.csv)", OT_ACTION_CMD_ImportTableFile, DataBase::instance().getSiteID());
}

void Model::importTableFile(const std::string &fileName, bool removeFile)
{
	TableReader reader;
	reader.setModel(this);
	ot::EntityCallbackBase cb;
	std::string error = reader.readFromFile(fileName, newTableItemName, this, getStateManager(), cb);

	if (removeFile)
	{
		std::remove(fileName.c_str());
	}

	if (!error.empty())
	{
		Application::instance()->getNotifier()->reportError(error);
	}
}

void Model::handleCreateNewParameter()
{
	enableQueuingHttpRequests(true);

	std::map<std::string, bool> allEntities = getListOfEntityNames();
	int count = 1;

	std::string parameterName;

	do
	{
		parameterName = getParameterRootName() + "/parameter" + std::to_string(count);
		count++;

	} while (allEntities.find(parameterName) != allEntities.end());

	EntityParameter *parameterItem = createNewParameterItem(parameterName);

	// Here we also need to add a new visualitation container item to the visualization model
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ParameterVisible";
	treeIcons.hiddenIcon = "ParameterHidden";
	Application::instance()->getNotifier()->addVisualizationContainerNode(visualizationModelID, parameterName, parameterItem->getEntityID(), treeIcons, parameterItem->getEditable());

	setModified();

	updatePropertyGrid(); // We need to update the property grid to make sure that the new material will become visible in the material selection

	modelChangeOperationCompleted("create new parameter");

	enableQueuingHttpRequests(false);
}

EntityParameter* Model::createNewParameterItem(const std::string &parameterName)
{
	EntityContainer *entityParameterRoot = dynamic_cast<EntityContainer*>(findEntityFromName(getParameterRootName()));
	if (entityParameterRoot == nullptr)
	{
		entityParameterRoot = new EntityContainer(createEntityUID(), nullptr, this, getStateManager());
		entityParameterRoot->setName(getParameterRootName());

		GeometryOperations::EntityList allNewEntities;
		addEntityToModel(entityParameterRoot->getName(), entityParameterRoot, entityRoot, true, allNewEntities);

		addVisualizationContainerNode(entityParameterRoot->getName(), entityParameterRoot->getEntityID(), entityParameterRoot->getEditable());
	}

	EntityParameter *parameterItem = new EntityParameter(createEntityUID(), entityParameterRoot, this, getStateManager());
	
	parameterItem->setName(parameterName);
	parameterItem->setEditable(true);
	parameterItem->createProperties();

	entityParameterRoot->addChild(parameterItem);

	addEntityToMap(parameterItem);

	return parameterItem;
}

void Model::setParameter(const std::string &name, double value, EntityParameter *parameter)
{
	std::string baseName = name.substr(getParameterRootName().size() + 1);
	parameterMap[baseName] = std::pair<double, EntityParameter*>(value, parameter);
}

void Model::removeParameter(const std::string &name)
{	
	std::string baseName = name.substr(getParameterRootName().size() + 1);
	parameterMap.erase(baseName);
}

void Model::setParameterDependency(std::list<std::string> &parameters, ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup)
{
	std::set<std::string> parameterSet;
	for (auto param : parameters) parameterSet.insert(param);

	// First, we remove this dependency from all parameters
	for (auto &parameter : parameterMap)
	{
		// Make sure that we don't want to add the dependency for this parameter again
		if (parameterSet.count(parameter.first) == 0)
		{
			parameter.second.second->removeDependency(entityID, propertyName, propertyGroup);
		}
	}

	// Now we add the dependency to the affected paramters
	for (auto &parameter : parameters)
	{
		parameterMap[parameter].second->addDependency(entityID, propertyName,propertyGroup);
	}
}

void Model::removeParameterDependency(ot::UID entityID)
{
	// Here we need to check whether we are currently removing a parameter. 
	EntityParameter *parameterEntity = dynamic_cast<EntityParameter *>(getEntityByID(entityID));		

	for (auto &parameter : parameterMap)
	{
		if (parameter.second.second->hasDependency(entityID))
		{
			if (parameterEntity != nullptr)
			{
				// The currently to be deleted parameter depends on the parameter -> remove te dependeny of the to be deleted parameter, but add all 
				// the parameter dependencies to the parent

				parameter.second.second->removeDependency(entityID);

				std::map<ot::UID, std::map<std::string, bool>> dependencyMap = parameterEntity->getDependencies();

				for (auto &entityDependency : dependencyMap)
				{
					for (auto &propertyDependency : entityDependency.second)
					{
						parameter.second.second->addDependencyByIndex(entityDependency.first, propertyDependency.first);
					}
				}
			}
			else
			{ 
				// The current entity depends on the parameter -> remove it from the parameters dependency list
				parameter.second.second->removeDependency(entityID);
			}
		}
	}

	// Check whether we are removing a parameter
	if (parameterEntity != nullptr)
	{
		removeParameter(parameterEntity->getName());

		// Now we need to replace the parameter by its value in all depending expressions
		replaceParameterByString(parameterEntity, "(" + parameterEntity->getValue() + ")");
	}
}

void Model::facetEntity(EntityGeometry *entity, double deflection, bool isHidden, bool notifyViewer)
{	
	double colorRGB[3] = { 0.8, 0.8, 0.8 };
	std::string name;

	EntityPropertiesColor *colorProp = dynamic_cast<EntityPropertiesColor*>(entity->getProperties().getProperty("Color"));
	assert(colorProp != nullptr);

	if (colorProp != nullptr)
	{
		colorRGB[0] = colorProp->getColorR();
		colorRGB[1] = colorProp->getColorG();
		colorRGB[2] = colorProp->getColorB();
	}

	std::string materialType = "Rough";

	EntityPropertiesSelection *typeProp = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Type"));

	if (typeProp != nullptr)
	{
		materialType = typeProp->getValue();
	}

	std::string textureType = "None";

	EntityPropertiesSelection *textureProp = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Texture"));

	if (textureProp != nullptr)
	{
		textureType = textureProp->getValue();
	}

	bool textureReflective = entity->isTextureReflective(textureType);

	entity->getFacets()->resetErrors();

	entity->getFacets()->getNodeVector().clear();
	entity->getFacets()->getTriangleList().clear();
	entity->getFacets()->getEdgeList().clear();

	GeometryOperations::facetEntity(entity->getBrep(), entity->getBrepEntity(), deflection, 
									entity->getFacets()->getNodeVector(), entity->getFacets()->getTriangleList(), entity->getFacets()->getEdgeList(), entity->getFacets()->getFaceNameMap(), entity->getFacets()->getErrorString());
	entity->getFacets()->setModified();

	entity->storeToDataBase();

	if (notifyViewer)
	{
		name = entity->getName();
		ot::UID modelEntityID = entity->getEntityID();

		std::vector<double> transformation;
		entity->getTransformation(transformation);

		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "ModelVisible";
		treeIcons.hiddenIcon = "ModelHidden";
		addVisualizationNodeFromFacetDataBase(name, colorRGB, colorRGB, materialType, textureType, textureReflective, modelEntityID, treeIcons, true, 1.0, isHidden, entity->getEditable(),
									  		  DataBase::instance().getCollectionName(), entity->getFacetsStorageObjectID(), entity->getSelectChildren(), entity->getManageParentVisibility(), entity->getManageChildVisibility(), entity->getShowWhenSelected(), transformation);

		if (!entity->getFacets()->getErrorString().empty()) displayMessage("Shape: " + name + "\n" + entity->getFacets()->getErrorString());
	}

	setModified();

	// We do not need to keep the facets in memory. Allow the object to clear memory data
	entity->releaseFacets();
}

void Model::addVisualizationNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], ot::UID modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
										      double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
											  bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	Application::instance()->getNotifier()->addVisualizationNodeFromFacetData(visualizationModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling, offsetFactor, isEditable, nodes, triangles, edges, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
}

void Model::addVisualizationNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool textureReflective, ot::UID modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
										          double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation
)
{
	ot::UID entityVersion = getStateManager()->getCurrentEntityVersion(entityID);
	Application::instance()->getNotifier()->addVisualizationNodeFromFacetDataBase(visualizationModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, textureReflective, modelEntityID, treeIcons, backFaceCulling, offsetFactor, isHidden, isEditable, projectName, entityID, entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
}

void Model::updateObjectFacetsFromDataBase(ot::UID modelEntityID, ot::UID entityID)
{
	ot::UID entityVersion = getStateManager()->getCurrentEntityVersion(entityID);
	Application::instance()->getNotifier()->updateObjectFacetsFromDataBase(getVisualizationModel(), modelEntityID, entityID, entityVersion);
}

double Model::calculateDeflectionFromAllEntities()
{
	std::list<EntityBase *> entities;
	getAllEntities(entities);

	return calculateDeflectionFromListOfEntities(entities);
}

double Model::calculateDeflectionFromListOfEntities(std::list<EntityBase *> &entities)
{
	// Determine the bounding box of the entity list
	BoundingBox boundingBox;

	for (auto entity : entities)
	{
		double xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0), zmin(0.0), zmax(0.0);
		entity->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax);

		boundingBox.extend(xmin, xmax, ymin, ymax, zmin, zmax);
	}

	// Now determine the deflection for facetting the entity based on the bounding box diagonal.
	double deflection = 0.0008 * boundingBox.getDiagonal();

	return deflection;
}

std::list<EntityBase *> Model::removeChildrenFromList(std::list<EntityBase *> selectedEntities)
{
	// The selectedEntities list may contain top level items and children altogether. The function will only add top level nodes to the list.

	// In a first step, bild a map with used entity pointers
	std::map<EntityBase *, bool> entityMap;
	for (auto entity : selectedEntities)
	{
		entityMap[entity] = true;
	}

	// Now loop through all items in the list and check whether a parent of the item is part of the list as well.
	// If not, add the entity to the new list of top level entities.

	std::list<EntityBase *> selectedTopLevelEntities;

	for (auto entity : selectedEntities)
	{
		bool parentFound = false;

		EntityBase *currentEntity = entity->getParent();

		while (currentEntity != nullptr)
		{
			if (entityMap.find(currentEntity) != entityMap.end())
			{
				parentFound = true;
				break;
			}

			currentEntity = currentEntity->getParent();
		}

		if (!parentFound) selectedTopLevelEntities.push_back(entity);
	}

	return selectedTopLevelEntities;
}

std::list<EntityBase*> Model::getListOfSelectedEntities(const std::string& typeFilter)
{
	std::list<EntityBase *> selectedEntities;

	for (ot::UID entityID : Application::instance()->getSelectionHandler().getSelectedEntityIDs())
	{
		EntityBase *entity = getEntityByID(entityID);

		if (typeFilter == "" || entity->getClassName() == typeFilter)
		{
			selectedEntities.push_back(entity);
		}
	}

	return selectedEntities;
}

void Model::addPropertiesToEntities(std::list<ot::UID>& entityIDList, const ot::PropertyGridCfg& _configuration)
{
	EntityProperties properties;
	properties.buildFromConfiguration(_configuration, getRootNode());

	std::list<EntityPropertiesBase*> allProperties = properties.getListOfAllProperties();

	bool anyPropertyAdded = false;

	// Now we loop through all entities
	for (auto entityID : entityIDList)
	{
		EntityBase* entity = getEntityByID(entityID);
		assert(entity != nullptr);

		for (auto prop : allProperties)
		{
			if (!entity->getProperties().propertyExists(prop->getName()))
			{
				// This property does not yet exist
				EntityPropertiesBase* newProperty = prop->createCopy();
				entity->getProperties().createProperty(newProperty, prop->getGroup());
				entity->setModified();

				anyPropertyAdded = true;
			}
		}
	}

	if (anyPropertyAdded)
	{
		updatePropertyGrid();
	}
}

void Model::updatePropertiesOfEntities(std::list<ot::UID>& entityIDList, const std::string& _entitiesJson)
{
	EntityProperties properties;
	properties.buildFromJSON(_entitiesJson, getRootNode());

	std::list<EntityPropertiesBase*> allProperties = properties.getListOfAllProperties();

	bool anyPropertyAdded = false;

	// Now we loop through all entities
	for (auto entityID : entityIDList)
	{
		EntityBase* entity = getEntityByID(entityID);
		assert(entity != nullptr);
		for (auto prop : allProperties)
		{
			EntityPropertiesBase* newProperty = prop->createCopy();
			entity->getProperties().updateProperty(newProperty, prop->getGroup());
			entity->setModified();
			anyPropertyAdded = true;
		}
	}
	
	if (anyPropertyAdded)
	{
		updatePropertyGrid();
	}
}

void Model::handleDeleteSelectedShapes()
{
	std::list<EntityBase *> selectedEntities;
	for (ot::UID entityID : Application::instance()->getSelectionHandler().getSelectedEntityIDs())
	{
		selectedEntities.push_back(entityMap[entityID]);
	}

	// No shapes are selected. Ignore
	if (selectedEntities.empty()) {
		return;
	}

	// Remove all protected entities from the list
	std::list<EntityBase *> selectedUnprotectedEntities;
	std::list<EntityBase *> protectedEntities;

	for (auto entity : selectedEntities)
	{
		if (!isProtectedEntity(entity))
		{
			selectedUnprotectedEntities.push_back(entity);
		}
		else
		{
			protectedEntities.push_back(entity);
		}
	}

	removeParentsOfProtected(selectedUnprotectedEntities,protectedEntities);

	// Now we remove all entities from the visualizaton and delete them afterward (if they are not protected)
	std::list<ot::UID> removeFromDisplay;

	// Remove all children from the list
	std::list<EntityBase *> selectedTopLevelEntities = removeChildrenFromList(selectedUnprotectedEntities);

	for (auto entity : selectedTopLevelEntities)
	{
		removeFromDisplay.push_back(entity->getEntityID());

		// Remove the entity from the entity map and also from the model state
		removeEntityFromMap(entity, false, false);
		delete entity;
	}

	removeFromDisplay.unique();

	removeShapesFromVisualization(removeFromDisplay);

	setModified();
	modelChangeOperationCompleted("delete objects");
}

void Model::removeShapesFromVisualization(std::list<ot::UID> &removeFromDisplay)
{
	if (visualizationModelID != 0)
	{
		Application::instance()->getNotifier()->removeShapesFromVisualization(visualizationModelID, removeFromDisplay);
	}
}

void Model::deleteEntity(EntityBase *entity)
{
	std::list<ot::UID> removeFromDisplay;
	removeFromDisplay.push_back(entity->getEntityID());

	// Remove the entity from the entity map and also delete it from the project state
	removeEntityFromMap(entity, false, false);

	delete entity;
	entity = nullptr;

	removeShapesFromVisualization(removeFromDisplay);
	setModified();
}

bool Model::isProtectedEntity(EntityBase *entity)
{
	if (entity->getParent() == nullptr) return true;
	if (entity->getParent() == entityRoot) return true;
	
	return !entity->deletable();
}

bool Model::anyMeshItemSelected(std::list<ot::UID> &selectedEntityID)
{
	for (auto entityID : selectedEntityID)
	{
		EntityMesh *entity = dynamic_cast<EntityMesh *> (entityMap[entityID]);
		if (entity != nullptr) return true;
	}

	return false;
}

bool Model::anyMaterialItemSelected(std::list<ot::UID> &selectedEntityID)
{
	for (auto entityID : selectedEntityID)
	{
		EntityMaterial *entity = dynamic_cast<EntityMaterial *> (entityMap[entityID]);

		if (entity != nullptr) return true;
	}

	return false;
}

void Model::modelItemRenamed(ot::UID entityID, const std::string &newName)
{
	EntityBase *entity = getEntityByID(entityID);
	assert(entity != nullptr);
	if (entity == nullptr) return;

	std::string fromPath = entity->getName();

	// Determine the full path name after the rename
	std::string toPath = entity->getName();
	size_t index = toPath.rfind("/");
	if (index == std::string::npos)
	{
		toPath = newName;
	}
	else
	{
		toPath = toPath.substr(0, index+1) + newName;
	}

	std::string error;

	// Check whether the name is empty
	if (newName.empty()) error = "The names must not be empty.";

	// Check whether the new name contains any invalid characters
	if (newName.find('/') != std::string::npos) error = "The names must not contain \"/\" characters.";

	// Check whether the new entity name is already in use
	if (findEntityFromName(toPath) != nullptr) error = "The new name is already in use.";

	EntityParameter *parameter = dynamic_cast<EntityParameter *>(entity);
	if (parameter != nullptr)
	{
		// Check the parameter name
		if (!checkParameterName(toPath.substr(getParameterRootName().size() + 1)))
		{
			error = "Invalid parameter name.";
		}
	}



	// Check whether we have encountered an error in one of the previous operations
	if (!error.empty())
	{
		reportError(error);

		ot::JsonDocument notify;
		notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_SetEntityName, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, entityID, notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, ot::JsonString(entity->getName(), notify.GetAllocator()), notify.GetAllocator());

		std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
		Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);
		return;
	}

	// Special handling in case we are renaming a parameter
	if (parameter != nullptr)
	{ 
		// Remove and re-add the parameter from the parameter map
		removeParameter(fromPath);
		setParameter(toPath, parameter->getNumericValue(), parameter);

		// Now we need to rename the parameter in all dependent expressions
		replaceParameterByString(parameter, toPath.substr(getParameterRootName().size() + 1));
	}

	// Rename the corresponding path in each entity name
	std::string filter = fromPath + "/";

	for (auto ent : entityMap)
	{
		EntityBase *item = ent.second;

		if (item->getName().size() == fromPath.size())
		{
			if (item->getName() == fromPath)
			{
				item->setName(toPath);
			}
		}
		else if (item->getName().size() > fromPath.size())
		{
			if (item->getName().substr(0, filter.size()) == filter)
			{
				std::string newName = toPath + item->getName().substr(filter.size() - 1);
				item->setName(newName);
			}
		}
	}

	modelChangeOperationCompleted("rename shapes");

	// Finally send the path rename operation to the entity
	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RenameEntityName, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_PATH_FROM, ot::JsonString(fromPath, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(toPath, notify.GetAllocator()), notify.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);

	EntityBlock* blockEntity = dynamic_cast<EntityBlock*>(entity);
	if (blockEntity != nullptr)
	{
		blockEntity->createBlockItem();
	}

	Application::instance()->getVisualisationHandler().handleRenaming(entityID);

}

void Model::keySequenceActivated(const std::string &keySequence) {

}

void Model::updatePropertyGrid()
{
	EntityProperties properties;
	ot::PropertyGridCfg cfg;
	const ot::UIDList& selectedVisibleEntityIDs = Application::instance()->getSelectionHandler().getSelectedVisibleEntityIDs();
	if (selectedVisibleEntityIDs.size() > 0)
	{
		this->addCommonPropertiesToConfig(selectedVisibleEntityIDs, true, cfg);
	}	
	else
	{
		const ot::UIDList& selectedEntityIDs = Application::instance()->getSelectionHandler().getSelectedEntityIDs();
		this->addCommonPropertiesToConfig(selectedEntityIDs, true, cfg);
	}

	m_selectedVersion.clear();
	Application::instance()->getNotifier()->fillPropertyGrid(cfg);
}

void Model::setEntityOutdated(EntityBase *entity)
{
	pendingEntityUpdates[entity] = true;
}

void Model::setEntityUpdated(EntityBase *entity)
{
	if (pendingEntityUpdates.count(entity) != 0)
	{
		pendingEntityUpdates.erase(entity);
	}
}

void Model::clearEntityUpdates(EntityBase *entity)
{
	pendingEntityUpdates.clear();
}

void Model::addCommonPropertiesToConfig(const std::list<ot::UID> &entityIDList, bool visibleOnly, ot::PropertyGridCfg& _config)
{
	std::list<EntityBase *> entities;
	for (auto entityID : entityIDList)
	{
		entities.push_back(getEntityByID(entityID));
	}

	EntityProperties props;
	getCommonProperties(entities, props);

	props.addToConfiguration(getRootNode(), visibleOnly, _config);
}

void Model::refreshAllViews()
{
	Application::instance()->getNotifier()->refreshAllViews(getVisualizationModel());
}

void Model::clearSelection()
{
	Application::instance()->getNotifier()->clearSelection(getVisualizationModel());
}

void Model::resetAllViews()
{
	Application::instance()->getNotifier()->resetAllViews(visualizationModelID);
}

void Model::setPropertiesFromJson(const std::list<ot::UID> &entityIDList, const ot::PropertyGridCfg& _configuration, bool update, bool itemsVisible)
{
	if (!m_selectedVersion.empty()) {
		this->setVersionPropertiesFromJson(_configuration);
	}
	else {
		std::list<EntityBase*> entities;
		for (auto entityID : entityIDList) entities.push_back(getEntityByID(entityID));

		EntityProperties properties;
		properties.buildFromConfiguration(_configuration, getRootNode());

		setProperties(entities, properties);

		if (update) {
			updateEntityProperties(itemsVisible);
		}
	}
}

void Model::setVersionPropertiesFromJson(const ot::PropertyGridCfg& _configuration) {
	if (m_selectedVersion.empty()) {
		OT_LOG_W("No version selected. Ignoring..");
		return;
	}

	ot::VersionGraphVersionCfg* version = getStateManager()->getVersionGraph().findVersion(m_selectedVersion);
	if (!version) {
		OT_LOG_EAS("Selected version not found \"" + m_selectedVersion + "\"");
		return;
	}

	std::list<ot::Property*> properties = _configuration.getAllProperties();
	for (ot::Property* prop : properties) {
		std::string path = prop->getPropertyPath();
		if (path == "Version/Label") {
			ot::PropertyString* actualProperty = dynamic_cast<ot::PropertyString*>(prop);
			if (!actualProperty) {
				OT_LOG_EAS("Property cast failed \"" + path + "\"");
				continue;
			}
			version->setLabel(actualProperty->getValue());
		}
		else {
			OT_LOG_EAS("Unknown version property \"" + path + "\"");
		}
	}

	getStateManager()->updateVersionEntity(m_selectedVersion);

	const ot::VersionGraphCfg& cfg = getStateManager()->getVersionGraph();
	this->sendVersionGraphToUI(cfg, cfg.getActiveVersionName(), cfg.getActiveBranchName());
}

void Model::setVersionLabel(const std::string& version, const std::string& label)
{
	ot::VersionGraphVersionCfg* versionObject = getStateManager()->getVersionGraph().findVersion(version);

	if (!versionObject) {
		OT_LOG_EAS("Selected version not found \"" + version + "\"");
		return;
	}

	versionObject->setLabel(label);

	getStateManager()->updateVersionEntity(version);

	const ot::VersionGraphCfg& cfg = getStateManager()->getVersionGraph();
	this->sendVersionGraphToUI(cfg, cfg.getActiveVersionName(), cfg.getActiveBranchName());
}

void Model::deleteProperty(const std::list<ot::UID> &entityIDList, const std::string& propertyName, const std::string& propertyGroup)
{
	std::list<EntityBase*> entities;
	for (auto entityID : entityIDList) entities.push_back(getEntityByID(entityID));

	std::list<EntityBase*> entitiesToSet = getListOfEntitiesToConsiderForPropertyChange(entities);

	bool needsUpdate = false;

	for (auto entity : entitiesToSet)
	{
		if (entity->getProperties().propertyExists(propertyName, propertyGroup))
		{
			// Check whether the property is deletable
			if (entity->getProperties().getProperty(propertyName, propertyGroup)->getProtected())
			{
				assert(0); // The property is protected, so we should not attempt to delete it
			}
			else
			{
				entity->getProperties().deleteProperty(propertyName, propertyGroup);
				entity->setModified();

				needsUpdate = true;
			}
		}
	}

	if (needsUpdate)
	{
		modelChangeOperationCompleted("Deleted property: " + propertyGroup + "/" + propertyName);
		updatePropertyGrid();
	}
}

void Model::getCommonProperties(const std::list<EntityBase *> &entities, EntityProperties &props)
{
	// Filter out properties which shall not be considered in this comparison
	// e.g. containers or children of certain other selected entities (e.g. mesh)

	// First bild a map of all selected entities
	std::map<EntityBase *, bool> entityMap;
	bool anyTopologyEntitySelected = false;
	for (auto entity : entities)
	{
		entityMap[entity] = true;
		if (entity->getEntityType() == EntityBase::TOPOLOGY)
		{
			if (dynamic_cast<EntityContainer *>(entity) != nullptr)
			{
				if (dynamic_cast<EntityContainer *>(entity)->getCreateVisualizationItem())
				{
					anyTopologyEntitySelected = true;
				}
			}
			else
			{
				anyTopologyEntitySelected = true;
			}
		}
	}

	// Now build a list with all entities for the comparison
	std::list<EntityBase *> entitiesToCompare;

	for (auto entity : entities)
	{
		// If any topology entity is selected, we just ignore the data entities 
		// This is relevant for 1d plot visualization where the curve entities are pointing to data entities
		// If only data enties are selected, then show their common properties
		bool isDataEntity = entity->getEntityType() == EntityBase::DATA;

		if (!isDataEntity)
		{
			if (dynamic_cast<EntityContainer *>(entity) != nullptr)
			{
				if (!dynamic_cast<EntityContainer *>(entity)->getCreateVisualizationItem())
				{
					isDataEntity = true;
				}
			}
		}

		if (!isDataEntity || !anyTopologyEntitySelected)
		{
			if (entity->considerForPropertyFilter())
			{
				// This entity is generally considered for the property filter.
				// We now need to check whether its parents are also in the list and if so, if one of them has the consider children flag set to false

				bool considerEntity = true;

				EntityBase *parent = entity->getParent();
				while (parent != nullptr)
				{
					if (entityMap.find(parent) == entityMap.end())
					{
						// This parent is not in the list of all items to compare -> stop the check here
						break;
					}

					if (!parent->considerChildrenForPropertyFilter())
					{
						considerEntity = false;
						break;
					}

					parent = parent->getParent();
				}

				if (considerEntity)
				{
					entitiesToCompare.push_back(entity);
				}
			}
		}
	}

	for (auto entity : entitiesToCompare)
	{
		if (entity == entitiesToCompare.front())
		{
			props = entity->getProperties();
		}
		else
		{
			entity->getProperties().checkMatchingProperties(props);
		}
	}
}

void Model::getEntityProperties(ot::UID entityID, bool recursive, const std::string& propertyGroupFilter, std::map<ot::UID, ot::PropertyGridCfg>& _entityProperties)
{
	// Get the specified entity
	EntityBase* entity = getEntityByID(entityID);

	// Add the properties of the given entity (and potentially its children) to the map
	getEntityProperties(entity, recursive, propertyGroupFilter, _entityProperties);
}

void Model::getEntityProperties(EntityBase* entity, bool recursive, const std::string& propertyGroupFilter, std::map<ot::UID, ot::PropertyGridCfg>& _entityProperties)
{
	if (entity != nullptr)
	{
		EntityProperties entityProps;

		for (auto property : entity->getProperties().getListOfAllProperties())
		{
			if (propertyGroupFilter.empty() || propertyGroupFilter == property->getGroup())
			{
				// We need to consider this property
				entityProps.createProperty(property->createCopy(), property->getGroup());
			}
		}

		if (entityProps.getNumberOfProperties() > 0)
		{
			ot::PropertyGridCfg cfg;
			entityProps.addToConfiguration(entity, false, cfg);
			_entityProperties[entity->getEntityID()] = cfg;
		}

		if (recursive)
		{
			EntityContainer* container = dynamic_cast<EntityContainer*>(entity);
			if (container != nullptr)
			{
				for (auto child : container->getChildrenList())
				{
					getEntityProperties(child, recursive, propertyGroupFilter, _entityProperties);
				}
			}
		}
	}
}


void Model::setShapeVisibility(std::list<ot::UID>& visibleEntityIDs, std::list<ot::UID>& hiddenEntityIDs)
{
	if (visualizationModelID != 0)
	{
		Application::instance()->getNotifier()->setShapeVisibility(visualizationModelID, visibleEntityIDs, hiddenEntityIDs);
	}
}

void Model::addTopologyEntitiesToModel(std::list<EntityBase*> _entities, const std::list<bool>& _forceVisible, bool _considerVisualization)
{
	std::map<ot::UID, EntityBase*> entityMap;
	std::map<EntityBase*, bool>forceEntityVisible;
	auto fvIterator = _forceVisible.begin();
	for (EntityBase* entity: _entities)
	{
		forceEntityVisible[entity] = *fvIterator;
		fvIterator++;
	}

	// We sort the entitylist by the length of the name. This ensures that potential parents are added before their childs are added
	_entities.sort([](EntityBase* e1, EntityBase* e2)
	{
		return e1->getName().size() < e2->getName().size();
	});

	// We need to get a list of all top level entities only
	std::list<EntityBase*> topLevelEntities = getTopLevelEntitiesByName(_entities);
	std::set<EntityBase*> topLevelEntitySet(std::begin(topLevelEntities), std::end(topLevelEntities));

	for (EntityBase* entity : _entities) {

		// We only need to add visualization containers above the top level entities if needed, since
		// all containers below the top level entities are part of the model and therefore are children of the top
		// level entities. These children will be added together with the entity below.
		
		bool addVisualizationContainer = false;

		if (topLevelEntitySet.find(entity) != topLevelEntitySet.end())
		{
			addVisualizationContainer = true;

			if (dynamic_cast<EntityContainer*>(entity) != nullptr)
			{
				addVisualizationContainer = dynamic_cast<EntityContainer*>(entity)->getCreateVisualizationItem();
			}
		}

		GeometryOperations::EntityList allNewEntities;
		addEntityToModel(entity->getName(), entity, entityRoot, addVisualizationContainer, allNewEntities);

		// Now the parent should not be empty
		ot::UID parentID = 0;
		if (entity->getParent() != nullptr)
		{
			parentID = entity->getParent()->getEntityID();
			assert(parentID != 0);
		}
		else
		{
			assert(0); // The entity should have a parent after insertion into the model
		}

		getStateManager()->storeEntity(entity->getEntityID(), parentID, entity->getEntityStorageVersion(), ModelStateEntity::tEntityType::TOPOLOGY);
	}
	
	// Might be that the visualisation of a parent entity depends on a child entity. Thus the children have to be part of the state before the visualisation is taken care of.
	// Here we will need to loop through the top level entities only, since they will automatically add their children.
	if (_considerVisualization) {
		for (EntityBase* entity : topLevelEntities) {
			entity->addVisualizationNodes();
		}
	}


	// Here we need to ensure that the entities with the force visible flag are visible

	ot::UIDList visibleEntityID;
	for (EntityBase* entity : _entities)
	{
		if (forceEntityVisible[entity] && entity->getInitiallyHidden())
		{
			// Ensure that this entity is visible
			visibleEntityID.push_back(entity->getEntityID());
		}
	}

	if (!visibleEntityID.empty())
	{
		ot::UIDList hiddenEntityID;
		setShapeVisibility(visibleEntityID, hiddenEntityID);
	}
}

std::list<EntityBase*> Model::getTopLevelEntitiesByName(std::list<EntityBase*> entities)
{
	std::list<EntityBase*> topLevelEntities;

	while (!entities.empty())
	{
		// We assume that the list of entities is sorted by name, so the first item is a top level entity
		EntityBase* thisTopLevelEntity = entities.front();

		topLevelEntities.push_back(thisTopLevelEntity);
		entities.pop_front();

		// Now we remove all children of the current toplevel entity and store the remaining entities in the otherEntities list
		std::list<EntityBase*> otherEntities;
		for (auto item : entities)
		{
			// Check whether the item is a child of the current toplevel entity
			if (item->getName().size() > thisTopLevelEntity->getName().size())
			{
				// This item can potentially be a child
				if (item->getName().substr(0, thisTopLevelEntity->getName().size() + 1) != thisTopLevelEntity->getName() + "/")
				{
					// This item is not a child of the toplevel entity
					otherEntities.push_back(item);
				}
			}
			else
			{
				// This item cannot be a child
				otherEntities.push_back(item);
			}
		}

		entities = otherEntities; // Now we have removed all children from the list
	}

	return topLevelEntities;
}

void Model::removeParentsOfProtected(std::list<EntityBase*>& unprotectedEntities, const std::list<EntityBase*>& protectedEntities)
{
	std::set<EntityBase*> unprotectedEntitySet(unprotectedEntities.begin(), unprotectedEntities.end());
	for (auto protectedEntity: protectedEntities)
	{
		EntityBase* currentParent = protectedEntity->getParent();

		while (currentParent != nullptr)
		{
			auto unprotectedParent = unprotectedEntitySet.find(currentParent);
			if (unprotectedParent != unprotectedEntitySet.end())
			{
				unprotectedEntities.remove(*unprotectedParent);
			}
			currentParent = currentParent->getParent();
		}
	}

}

std::list<EntityBase*> Model::FindTopLevelBlockEntities(std::list<EntityBase*>& allEntitiesForDeletion)
{
	//This function sorts out the block entities and return only the top level block entities.
	// 
	// In a first step, bild a map with used entity pointers
	std::map<EntityBase*, bool> entityMap;
	for (auto entity : allEntitiesForDeletion)
	{
		entityMap[entity] = true;
	}

	std::list<EntityBase*> selectedTopLevelBlockEntities;

	for (auto entity : allEntitiesForDeletion)
	{
		const bool entityIsABlockEntity = dynamic_cast<EntityBlock*>(entity) != nullptr;
		if (entityIsABlockEntity)
		{
			// Now check whether a parent of the item is part of the list as well and if this parent is also a block entity.
			// If not, add the entity to the new list of top level entities.
			bool parentFoundAndIsBlock = false;

			EntityBase* currentEntity = entity->getParent();

			while (currentEntity != nullptr)
			{
				if (entityMap.find(currentEntity) != entityMap.end())
				{
					const bool parentIsABlockEntity = dynamic_cast<EntityBlock*>(currentEntity) != nullptr;
					if (parentIsABlockEntity)
					{
						parentFoundAndIsBlock = true;
						break;
					}
					else
					{
						break;
					}
				}

				currentEntity = currentEntity->getParent();
			}
		
			if (!parentFoundAndIsBlock) selectedTopLevelBlockEntities.push_back(entity);
		}
	}

	return selectedTopLevelBlockEntities;
}

std::list<EntityBase*> Model::getListOfEntitiesToConsiderForPropertyChange(const std::list<EntityBase*>& entities)
{
	// Filter out properties which shall not be considered in this comparison
// e.g. containers or children of certain other selected entities (e.g. mesh)

// First bild a map of all selected entities
	std::map<EntityBase*, bool> entityMap;
	bool anyTopologyEntitySelected = false;
	for (auto entity : entities)
	{
		entityMap[entity] = true;
		if (entity->getEntityType() == EntityBase::TOPOLOGY)
		{
			if (dynamic_cast<EntityContainer*>(entity) != nullptr)
			{
				if (dynamic_cast<EntityContainer*>(entity)->getCreateVisualizationItem())
				{
					anyTopologyEntitySelected = true;
				}
			}
			else
			{
				anyTopologyEntitySelected = true;
			}
		}
	}

	// Now build a list with all entities for the comparison
	std::list<EntityBase*> entitiesToSet;

	for (auto entity : entities)
	{
		// If any topology entity is selected, we just ignore the data entities 
		// This is relevant for 1d plot visualization where the curve entities are pointing to data entities
		// If only data enties are selected, then show their common properties
		bool isDataEntity = entity->getEntityType() == EntityBase::DATA;

		if (!isDataEntity)
		{
			if (dynamic_cast<EntityContainer*>(entity) != nullptr)
			{
				if (!dynamic_cast<EntityContainer*>(entity)->getCreateVisualizationItem())
				{
					isDataEntity = true;
				}
			}
		}

		if (!isDataEntity || !anyTopologyEntitySelected)
		{
			if (entity->considerForPropertyFilter())
			{
				// This entity is generally considered for the property filter.
				// We now need to check whether its parents are also in the list and if so, if one of them has the consider children flag set to false

				bool considerEntity = true;

				EntityBase* parent = entity->getParent();
				while (parent != nullptr)
				{
					if (entityMap.find(parent) == entityMap.end())
					{
						// This parent is not in the list of all items to compare -> stop the check here
						break;
					}

					if (!parent->considerChildrenForPropertyFilter())
					{
						considerEntity = false;
						break;
					}

					parent = parent->getParent();
				}

				if (considerEntity)
				{
					entitiesToSet.push_back(entity);
				}
			}
		}
	}

	return entitiesToSet;
}

void Model::setProperties(const std::list<EntityBase *> &entities, EntityProperties &props)
{
	std::list<EntityBase*> entitiesToSet = getListOfEntitiesToConsiderForPropertyChange(entities);

	for (auto entity : entitiesToSet)
	{
		entity->getProperties().readFromProperties(props, getRootNode());

		if (entity->getProperties().anyPropertyNeedsUpdate())
		{
			setEntityOutdated(entity);
			entity->setModified();
		}
	}
}

bool Model::entitiesNeedUpdate()
{
	return (!pendingEntityUpdates.empty());
}

void Model::updateEntityProperties(bool itemsVisible)
{
	enableQueuingHttpRequests(true);

	std::map<std::string, std::list<std::pair<ot::UID, ot::UID>>> otherServicesUpdate;

	bool oldWriteQueueingState = DataBase::instance().isQueueWritingEnabled();
	DataBase::instance().setWritingQueueEnabled(true);

	bool needsWritingQueueFlush = false;

	bool anyEntityNeedsUpdate = !pendingEntityUpdates.empty();
	
	while (!pendingEntityUpdates.empty())
	{
		// Now we try to pick a paramter from the list of pending entity updates, such that the parameters are 
		// always updated first. If there is no parameter to update, just pich the first entity from the map.

		EntityBase * entity = nullptr;
		
		// Try for find a paramter
		for (auto &ent : pendingEntityUpdates)
		{
			if (dynamic_cast<EntityParameter *>(ent.first) != nullptr)
			{
				// We have found a parameter
				entity = ent.first;
				break;
			}
		}
		
		if (entity == nullptr)
		{
			// No parameter found, pick the first entity
			entity = pendingEntityUpdates.begin()->first;
		}

		updateEntity(entity);
		setEntityUpdated(entity);

		bool stored = false;

		for (const std::string& cbService : entity->getServicesForCallback(EntityBase::Callback::Properties)) {
			if (!stored) {
				// Store the entity to the database only once
				entity->storeToDataBase();
				needsWritingQueueFlush = true;
				stored = true;
			}
			otherServicesUpdate[cbService].push_back(std::pair<ot::UID, ot::UID>(entity->getEntityID(), entity->getEntityStorageVersion()));
		}

		if (dynamic_cast<EntityParameter *>(entity) != nullptr)
		{
			// We are updating a parameter. Set all dependent properties to modified
			EntityParameter* parameter = dynamic_cast<EntityParameter *>(entity);
			std::map<ot::UID, std::map<std::string, bool>> dependencyMap = parameter->getDependencies();

			for (auto &entityDependency : dependencyMap)
			{
				EntityBase* dependentEntity = getEntityByID(entityDependency.first);

				for (auto &propertyDependency : entityDependency.second)
				{
					dependentEntity->getProperties().getProperty(propertyDependency.first)->setNeedsUpdate();
				}

				dependentEntity->setModified();
				setEntityOutdated(dependentEntity);
			}
		}
	}

	if (needsWritingQueueFlush)
	{
		DataBase::instance().flushWritingQueue();
	}

	DataBase::instance().setWritingQueueEnabled(oldWriteQueueingState);

	if (otherServicesUpdate.empty())
	{
		if (anyEntityNeedsUpdate)
		{
			refreshAllViews();
			modelChangeOperationCompleted("shape properties changed");
		}

		enableQueuingHttpRequests(false);
	}
	else
	{
		enableQueuingHttpRequests(false);

		// Now run a parallel thread for performing the additional services update. Running these updates in a parallel 
		// thread ensures that the model service is not blocked and remains responsive to other services requests.
		// This thread needs to send a XXX message at the end.

		std::thread workerThread(&Model::otherServicesUpdate, this, otherServicesUpdate, itemsVisible);
		workerThread.detach();
	}
}

void Model::otherServicesUpdate(std::map<std::string, std::list<std::pair<ot::UID, ot::UID>>> otherServicesUpdate, bool itemsVisible)
{
	ot::LockTypes lockFlag(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);
	ot::UILockWrapper uiLock(Application::instance()->getUiComponent(), lockFlag);

	for (const auto& serviceUpdate : otherServicesUpdate)
	{
		if (serviceUpdate.first == Application::instance()->getServiceName()) {
			continue;
		}

		std::list<ot::UID> entityIDs, entityVersions, brepVersions;
		ot::JsonArray changedEntitiesInfos;
		ot::JsonDocument notify;
		for (const auto& entity : serviceUpdate.second) {
			ot::UID brepVersion = 0;

			EntityGeometry *geomEntity = dynamic_cast<EntityGeometry *>(getEntityByID(entity.first));
			if (geomEntity != nullptr)
			{
				brepVersion = getStateManager()->getCurrentEntityVersion(geomEntity->getBrepStorageObjectID());
			}

			entityIDs.push_back(entity.first);
			entityVersions.push_back(entity.second);
			brepVersions.push_back(brepVersion);
			auto changedEntityByID = entityMap.find(entity.first);
			assert(changedEntityByID != entityMap.end());
			ot::EntityInformation entityInfo(changedEntityByID->second);
			ot::JsonObject entityInfoSerialised;
			entityInfo.addToJsonObject(entityInfoSerialised, notify.GetAllocator());
			changedEntitiesInfos.PushBack(entityInfoSerialised, notify.GetAllocator());
		}


		notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_PropertyChanged, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_EntityInfo, changedEntitiesInfos, notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDs, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersions, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_BrepVersionList, ot::JsonArray(brepVersions, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, itemsVisible, notify.GetAllocator());

		Application::instance()->getNotifier()->sendMessageToService(false, serviceUpdate.first, notify);
	}

	// Now we need to notify the model service that the update operation is completed
	refreshAllViews();
	modelChangeOperationCompleted("shape properties changed");
}

void Model::updateEntity(EntityBase *entity)
{
	// In a first step, we need to update all numerical values which might depend on parameters
	bool updatePropertiesGrid = updateNumericalValues(entity);

	// Now perform the entity specific updates
	updatePropertiesGrid |= entity->updateFromProperties();
	performSpecialUpdates(entity);

	// Now we need to update the status of the Property Updates
	entity->getProperties().checkWhetherUpdateNecessary();

	// Now check whether there are still any missing updates (other services may still take care of updating their entities
	assert(!entity->getProperties().anyPropertyNeedsUpdate());

	// If there are any changes in the properties due to the update, we need to update the property grid
	if (updatePropertiesGrid)
	{
		updatePropertyGrid();
	}
}

bool Model::updateNumericalValues(EntityBase *entity)
{
	bool needsPropertyGridUpdate = false;

	// We loop through all properties and check the ones which are marked by "#" in front

	std::list<EntityPropertiesDouble *> numericalProperties = entity->getProperties().getListOfNumericalProperties();

	for (auto property : numericalProperties)
	{
		std::string expressionPropertyName = property->getName().substr(1);
		EntityPropertiesString *expressionProperty = dynamic_cast<EntityPropertiesString *>(entity->getProperties().getProperty(expressionPropertyName));

		if (expressionProperty != nullptr)
		{
			if (expressionProperty->needsUpdate())
			{
				double value = 0.0;

				if (evaluateExpressionDouble(expressionProperty->getValue(), value, entity->getEntityID(), expressionProperty->getName(), expressionProperty->getGroup()))
				{
					property->setValue(value);  // This will also set the needsUpdate flag for the property if the value is different

					// Reset the error state of needed
					if (expressionProperty->getErrorState())
					{					
						needsPropertyGridUpdate = true;
						expressionProperty->setErrorState(false);
					}

					expressionProperty->resetNeedsUpdate();
				}
				else
				{
				// Error in evaluating expression 
				expressionProperty->setErrorState(true);
				needsPropertyGridUpdate = true;
				}
			}
		}
	}

	return needsPropertyGridUpdate;
}

bool Model::checkParameterName(const std::string &parameterName)
{
	if (parameterName.empty()) return false;

	// First check whether the parameter name contains any unallowed characters
	for (size_t index = 0; index < parameterName.size(); index++)
	{
		if (!isValidParameterNameCharacter(parameterName[index]))
		{
			return false;
		}
	}

	// Now check whether the parameter starts with a number
	if (parameterName[0] >= '0' && parameterName[0] <= '9')
	{
		return false;
	}

	// The name seems to be correct
	return true;
}

bool Model::isValidParameterNameCharacter(char c)
{
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= '0' && c <= '9') return true;
	if (c == '_') return true;

	return false;
}

void Model::replaceParameterByString(EntityParameter *parameter, const std::string &value)
{
	// We need to search for all dependenices on this parameter and replace the parameter name by the string in all dependent properties.

	std::map<ot::UID, std::map<std::string, bool>> dependencyMap = parameter->getDependencies();

	for (auto &entityDependency : dependencyMap)
	{
		EntityBase *entity = getEntityByID(entityDependency.first);
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			for (auto &propertyDependency : entityDependency.second)
			{
				EntityPropertiesString *expressionProperty = dynamic_cast<EntityPropertiesString *>(entity->getProperties().getProperty(propertyDependency.first));
				assert(expressionProperty != nullptr);

				if (expressionProperty != nullptr)
				{
					expressionProperty->setValue(replaceParameterInExpression(expressionProperty->getValue(), parameter->getName().substr(getParameterRootName().size() + 1), value));
					expressionProperty->resetNeedsUpdate();  // The value should not have changed

					entity->setModified();  // But we need to store the entity again
				}
			}
		}
	}
}

std::string Model::replaceParameterInExpression(std::string expression, const std::string &parameterName, const std::string &newString)
{
	if (parameterName == newString) return expression;

	size_t offset = 0;

	while (1)
	{
		size_t index = expression.find(parameterName, offset);
		if (index != std::string::npos)
		{
			bool validCatch = true;

			// We need to check the character on the left and the one on the right to ensure that they are not valid characters for parameter names
			// (otherwise the found string might just be a substring of a longer name)
			if (index > 0)
			{
				if (isValidParameterNameCharacter(expression[index - 1]))
				{
					// It seems that the current catch is just a substring -> discard
					validCatch = false;
				}
			}

			if (index+parameterName.size() < expression.size())
			{
				if (isValidParameterNameCharacter(expression[index+parameterName.size()]))
				{
					// It seems that the current catch is just a substring -> discard
					validCatch = false;
				}
			}

			if (validCatch)
			{
				// We have found an occurence of the parameter -> replace the string
				expression.replace(index, parameterName.size(), newString);
				offset += newString.size();
			}
			else
			{
				offset += parameterName.size();
			}
		}
		else
		{
			// No further occurences found
			return expression;
		}
	}

	assert(0); // Something went wrong
	return "";
}

bool Model::evaluateExpressionDouble(const std::string &expression, double &value, ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup)
{
	int nParameter = (int)parameterMap.size();
	te_variable *parameter = nullptr;

	if (nParameter > 0)
	{
		parameter = new te_variable[nParameter];
		int index = 0;
		for (auto &par : parameterMap)
		{
			parameter[index].name = par.first.c_str();
			parameter[index].address = &(par.second.first);
			parameter[index].type = 0;
			parameter[index].context = 0;

			index++;
		}
	}

	int error{ 0 };
	te_expr *expr = te_compile(expression.c_str(), parameter, nParameter, &error);

	if (expr != nullptr)
	{
		value = te_eval(expr);

		// Now update the dependencies
		std::list<std::string> parameters;
		for (struct te_varUsed *var = te_getVariablesUsed(); var != NULL; var = var->next)
		{
			parameters.push_back(var->name);
		}

		if (!parameters.empty() && dynamic_cast<EntityParameter *>(getEntityByID(entityID)) != nullptr)
		{
			// We are updating a paramter -> check for circular dependency
			if (checkCircularParameterDependency(dynamic_cast<EntityParameter *>(getEntityByID(entityID)), parameters))
			{
				// We do have a circular dependency -> error
				te_free(expr);

				if (parameter != nullptr)
				{
					delete[] parameter;
				}

				std::string message;
				message = "*** ERROR *** Circular dependency detected in expression for parameter: " + getEntityByID(entityID)->getName() + "\n\n";

				displayMessage(message);

				return false;
			}
		}

		setParameterDependency(parameters, entityID, propertyName, propertyGroup);

		te_free(expr);

		if (parameter != nullptr)
		{
			delete[] parameter;
		}

		return true; // Successful evaluation
	}

	if (parameter != nullptr)
	{
		delete[] parameter;
	}

	std::string message, indicator;
	message = "*** ERROR *** Invalid expression : " + expression + "\n";
	indicator.resize(error + 34, ' ');
	message += indicator + "^\n\n";

	displayMessage(message);

	return false; // Error in evaluating the expression
}

bool Model::checkCircularParameterDependency(EntityParameter *parameter, std::list<std::string> &dependingOnParameters)
{
	// We need to check whether any of the paramters on which we are depending depends on us, too

	for (auto &dependingOnParameter : dependingOnParameters)
	{
		EntityParameter *checkPar = dynamic_cast<EntityParameter *>(findEntityFromName(getParameterRootName() + "/" + dependingOnParameter));
		assert(checkPar != nullptr);

		if (checkPar == parameter)
		{
			// The parameter is depending on itself
			return true;
		}
		
		if (checkPar != nullptr)
		{
			if (checkWhetherParameterDependsOnGivenParameter(parameter, checkPar->getEntityID()))
			{
				return true; // We have a circular dependency
			}
		}
	}

	return false; // We did not find a circular dependency
}

bool Model::checkWhetherParameterDependsOnGivenParameter(EntityParameter *parameter, ot::UID dependingEntityID)
{
	std::map<ot::UID, std::map<std::string, bool>> dependencyMap = parameter->getDependencies();

	// First check whether the parameter is directly depending on the given entityID
	
	if (dependencyMap.count(dependingEntityID) > 0)
	{
		return true;  // This parameter is depending on the given entity
	}

	// Now check whether there are any other parameters on which we are depending. If so, check their dependency
	for (auto &dependingOnEntity : dependencyMap)
	{
		EntityParameter *dependingOnParameter = dynamic_cast<EntityParameter *>(getEntityByID(dependingOnEntity.first));

		if (dependingOnParameter != nullptr)
		{
			// We are depending on a parameter
			if (checkWhetherParameterDependsOnGivenParameter(dependingOnParameter, dependingEntityID))
			{
				// We are indirectly depending on the given entity
				return true;
			}
		}
	}

	// We are not depending on the given entity
	return false; 
}

void Model::performSpecialUpdates(EntityBase *entity)
{
	if (dynamic_cast<EntityMeshTet*>(entity) != nullptr)
	{
		performEntityMeshUpdate(dynamic_cast<EntityMeshTet*>(entity));
		return;
	}
	
	if (dynamic_cast<EntityMeshCartesian*>(entity) != nullptr)
	{
		performEntityMeshUpdate(dynamic_cast<EntityMeshCartesian*>(entity));
		return;
	}
	
	if (dynamic_cast<EntityFaceAnnotation*>(entity) != nullptr)
	{
		updateAnnotationGeometry(dynamic_cast<EntityFaceAnnotation*>(entity));
		return;
	}
	
	if (dynamic_cast<EntityParameter*>(entity) != nullptr)
	{
		setParameter(entity->getName(), dynamic_cast<EntityParameter*>(entity)->getNumericValue(), dynamic_cast<EntityParameter*>(entity));
		return;
	}
}

void Model::performEntityMeshUpdate(EntityMeshTet *entity)
{
	entity->setMeshValid(false);
	bool updatePropertiesGrid = false;

	// Check whether the bounding sphere mode has changed
	EntityPropertiesSelection *boundingSphereMode = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Bounding sphere mode"));
	if (boundingSphereMode != nullptr)
	{
		if (boundingSphereMode->needsUpdate())
		{
			EntityPropertiesDouble *boundingSphereRadiusProperty = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Bounding sphere radius"));
			assert(boundingSphereRadiusProperty != nullptr);

			std::list<EntityGeometry *> geometryEntities;
			getAllGeometryEntities(geometryEntities);

			BoundingBox boundingBox = GeometryOperations::getBoundingBox(geometryEntities);

			if (boundingBox.getDiagonal() == 0.0)
			{
				boundingSphereRadiusProperty->setValue(0.0);
			}
			else
			{
				if (boundingSphereMode->getValue() == "Relative")
				{
					// We have changed from absoute to relative values
					boundingSphereRadiusProperty->setValue(2.0 * boundingSphereRadiusProperty->getValue() / boundingBox.getDiagonal());
				}
				else
				{
					// We have changed from relative to absolute values
					boundingSphereRadiusProperty->setValue(0.5 * boundingSphereRadiusProperty->getValue() * boundingBox.getDiagonal());
				}
			}
			updatePropertiesGrid = true;
		}
	}

	updatePropertiesGrid |= entity->updatePropertyVisibilities();

	entity->getProperties().forceResetUpdateForAllProperties();

	if (updatePropertiesGrid)
	{
		updatePropertyGrid();
	}
}

void Model::performEntityMeshUpdate(EntityMeshCartesian *entity)
{
	entity->setMeshValid(false);
	bool updatePropertiesGrid = false;

	updatePropertiesGrid |= entity->updatePropertyVisibilities();

	entity->getProperties().forceResetUpdateForAllProperties();

	if (updatePropertiesGrid)
	{
		updatePropertyGrid();
	}
}

void Model::addAllEntitiesToList(EntityBase *root, std::list<EntityBase *> &allEntities)
{
	if (dynamic_cast<EntityContainer *>(root) == nullptr)
	{
		allEntities.push_back(root);
		return;
	}

	// Here we have an item which is inherited from a container. Since we want to ignore pure entity container items only, we  
	// check the type and add the item to the list if not an EntityContainer

	if (root->getClassName() != "EntityContainer")
	{
		allEntities.push_back(root);
	}

	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			addAllEntitiesToList(child, allEntities);
		}
	}
}

void Model::addGeometryEntitiesToList(EntityBase *root, std::list<EntityGeometry *> &geometryEntities)
{
	if (dynamic_cast<EntityGeometry *>(root) != nullptr)
	{
		geometryEntities.push_back(dynamic_cast<EntityGeometry *>(root));
		return;
	}

	if (root->getClassName() != "EntityGeometry")
	{
		EntityContainer *container = dynamic_cast<EntityContainer *>(root);

		if (container != nullptr)
		{
			for (auto child : container->getChildrenList())
			{
				addGeometryEntitiesToList(child, geometryEntities);
			}
		}
	}
}

void Model::getAllGeometryEntities(std::list<EntityGeometry *> &geometryEntities)
{
	geometryEntities.clear();

	EntityBase *entityGeometryRoot = findEntityFromName(getGeometryRootName());
	assert(entityGeometryRoot != nullptr);

	addGeometryEntitiesToList(entityGeometryRoot, geometryEntities);
}

void Model::getAllEntities(std::list<EntityBase *> &allEntities)
{
	allEntities.clear();
	addAllEntitiesToList(entityRoot, allEntities);
}

void Model::displayMessage(const std::string &message)
{
	Application::instance()->getNotifier()->displayMessage(message);
}

void Model::reportError(const std::string &message)
{
	Application::instance()->getNotifier()->reportError(message);
}

void Model::reportWarning(const std::string &message)
{
	Application::instance()->getNotifier()->reportWarning(message);
}

void Model::reportInformation(const std::string &message)
{
	Application::instance()->getNotifier()->reportInformation(message);
}

void Model::handleShowSelectedShapeInformation()
{
	std::list<EntityBase *> selectedEntities;
	for (ot::UID entityID : Application::instance()->getSelectionHandler().getSelectedEntityIDs())
	{
		selectedEntities.push_back(entityMap[entityID]);
	}

	// We need to ensure that we have all necessary geometry information loaded (prefetching)
	std::list<ot::UID> prefetchIds;

	for (auto entity : selectedEntities)
	{
		if (dynamic_cast<EntityGeometry*>(entity))
		{
			EntityGeometry *geometry = dynamic_cast<EntityGeometry*>(entity);
			geometry->addPrefetchingRequirementsForBrep(prefetchIds);
		}
	}

	prefetchDocumentsFromStorage(prefetchIds);

	// Now extract the geometry information
	bool boxSet = false;
	double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0, zmin = 0.0, zmax = 0.0;

	size_t numberTets = 0;
	size_t numberTriangles = 0;

	std::string meshElementsWithoutTriangles;
	std::string meshElementsWithoutTets;

	std::list<double> faceCurvatureRadius;

	std::list<EntityGeometry *> geometryEntities;

	std::string additionalInformation("");

	for (auto entity : selectedEntities)
	{
		double lxmin = 0.0, lxmax = 0.0, lymin = 0.0, lymax = 0.0, lzmin = 0.0, lzmax = 0.0;

		if (entity->getEntityBox(lxmin, lxmax, lymin, lymax, lzmin, lzmax))
		{
			if (!boxSet)
			{
				boxSet = true;

				xmin = lxmin;
				xmax = lxmax;
				ymin = lymin;
				ymax = lymax;
				zmin = lzmin;
				zmax = lzmax;
			}
			else
			{
				xmin = std::min(xmin, lxmin);
				xmax = std::max(xmax, lxmax);
				ymin = std::min(ymin, lymin);
				ymax = std::max(ymax, lymax);
				zmin = std::min(zmin, lzmin);
				zmax = std::max(zmax, lzmax);
			}
		}

		if (dynamic_cast<EntityMeshTetItem*>(entity))
		{
			EntityMeshTetItem *meshItem = dynamic_cast<EntityMeshTetItem*>(entity);

			numberTets += meshItem->getNumberTets();
			numberTriangles += meshItem->getNumberTriangles();

			if (meshItem->getNumberTets() == 0)
			{
				meshElementsWithoutTets += "   " + entity->getName() + "\n";
			}

			if (meshItem->getNumberFaces() == 0)
			{
				meshElementsWithoutTriangles += "   " + entity->getName() + "\n";
			}
		}

		if (dynamic_cast<EntityGeometry*>(entity))
		{
			EntityGeometry *geometry = dynamic_cast<EntityGeometry*>(entity);
			getFaceCurvatureRadius(&(geometry->getBrep()), faceCurvatureRadius);

			// Add the entity to the list for determining the number of triangles
			geometryEntities.push_back(geometry);
		}
	
		EntityMetadataSeries* series =	dynamic_cast<EntityMetadataSeries*>(entity);
		
		if (series != nullptr)
		{
			MetadataEntityInterface metadataInterface;
			MetadataSeries seriesMetadata=	metadataInterface.createSeries(series);
			
			ot::JsonDocument document;
			seriesMetadata.addToJsonObject(document, document.GetAllocator());
			
			additionalInformation += ot::json::toPrettyString(document);
		}

	}
	
	std::string message;
	message = "____________________________________________________________\n\n";
	message += "Selected object information: \n\n";
	message += "  Number of selected objects: " + std::to_string(selectedEntities.size()) + "\n\n";

	// Now check whether one parameter is selected
	for (auto entity : selectedEntities)
	{
		EntityParameter *parameter = dynamic_cast<EntityParameter *>(entity);
		if (parameter != nullptr)
		{
			message += "  Dependencies on parameter \"" + parameter->getName().substr(getParameterRootName().size() + 1) + "\":\n";

			std::map<ot::UID, std::map<std::string, bool>> dependencyMap = parameter->getDependencies();

			if (dependencyMap.empty())
			{
				message += "    none\n";
			}
			else
			{
				for (auto &entityDependency : dependencyMap)
				{
					message += "    " + getEntityByID(entityDependency.first)->getName() + "\n";

					for (auto &propertyDependency : entityDependency.second)
					{
						message += "      " + propertyDependency.first + "\n";
					}
				}
			}

			message += "\n";
		}
	}

	if (!additionalInformation.empty())
	{
		message += additionalInformation;
	}

	if (boxSet)
	{
		message += "  X-min/max: " + std::to_string(xmin) + " - " + std::to_string(xmax) + "     Delta X: " + std::to_string(xmax-xmin) + "\n";
		message += "  Y-min/max: " + std::to_string(ymin) + " - " + std::to_string(ymax) + "     Delta Y: " + std::to_string(ymax-ymin) + "\n";
		message += "  Z-min/max: " + std::to_string(zmin) + " - " + std::to_string(zmax) + "     Delta Z: " + std::to_string(zmax-zmin) + "\n\n";

		double diag = sqrt( (xmax - xmin) * (xmax - xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

		message += "  Diagonal: " + std::to_string(diag) + "\n\n";
	}

	size_t numberVisualizationTriangles = getNumberOfVisualizationTriangles(geometryEntities);

	if (numberVisualizationTriangles > 0)
	{
		message += "  Number of visualization triangles: " + std::to_string(numberVisualizationTriangles) + "\n\n";
	}

	if (numberTriangles > 0 || numberTets > 0)
	{
		message += "  Number of triangles: " + std::to_string(numberTriangles) + "\n";
		message += "  Number of tetrahedrons: " + std::to_string(numberTets) + "\n\n";

		if (meshElementsWithoutTriangles != "")
		{
			message += "  Mesh objects without triangles: \n" + meshElementsWithoutTriangles + "\n";
		}

		if (meshElementsWithoutTets != "")
		{
			message += "  Mesh objects without tetrahedrons: \n" + meshElementsWithoutTets + "\n";
		}
	}

	if (!faceCurvatureRadius.empty())
	{
		std::vector<std::pair<std::pair<double, double>, int>> curvatureHistogram = GeometryOperations::getCurvatureRadiusHistogram(faceCurvatureRadius, 20);

		message += "  Curvature radius:\n";

		for (int i = 0; i < curvatureHistogram.size(); i++)
		{
			std::pair<std::pair<double, double>, int> item = curvatureHistogram[i];
			std::string line;
			line = "    [" + std::to_string(item.first.first) + ", " + std::to_string(item.first.second) + "]  :  " + std::to_string(item.second) + "\n";
			message += line;
		}
	}

	message += "\n";

	displayMessage(message);
}

size_t Model::getNumberOfVisualizationTriangles(std::list<EntityGeometry *> geometryEntities)
{
	if (geometryEntities.empty()) return 0;

	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::instance().getCollectionName());
	DataStorageAPI::QueryBuilder queryBuilder;

	std::vector<std::string> columnNames;
	columnNames.push_back("NumberOfTriangles");

	auto query = bsoncxx::builder::basic::document{};
	auto queryArray = bsoncxx::builder::basic::array();

	for (auto entity : geometryEntities)
	{
		unsigned long long entityID = entity->getFacetsStorageObjectID();
		unsigned long long entityVersion = stateManager->getCurrentEntityVersion(entityID);

		assert(entityVersion != 1); // This is for testing whether still some old version convention is being used somewhere

		auto builder = bsoncxx::builder::basic::document{};
		builder.append(bsoncxx::builder::basic::kvp("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
		builder.append(bsoncxx::builder::basic::kvp("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

		queryArray.append(builder);
	}

	auto queryBuilderDoc = bsoncxx::builder::basic::document{};
	queryBuilderDoc.append(kvp("$or", queryArray));

	BsonViewOrValue filterQuery = queryBuilderDoc.extract();
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	auto results = docBase.GetAllDocument(std::move(filterQuery), std::move(projectionQuery), 0);

	// Now results is an array with all documents found

	size_t numberTriangles = 0;

	for (auto result : results)
	{
		try
		{
			numberTriangles += result["NumberOfTriangles"].get_int64().value;
		}
		catch (std::exception)
		{
			return 0; // At least one document does not have a triangle count assigned
		}
	}

	return numberTriangles;
}

void Model::addAnnotationEntities(std::list<EntityAnnotation *> &errorAnnotations)
{
	GeometryOperations::EntityList allNewEntities;

	for (auto annotation : errorAnnotations)
	{
		addEntityToModel(annotation->getName(), annotation, entityRoot, true, allNewEntities);

		annotation->addVisualizationItem(false);
	}
}

void Model::addVisualizationAnnotationNode(const std::string &name, ot::UID UID, const OldTreeIcon &treeIcons, bool isHidden,
										   const double edgeColorRGB[3],
										   const std::vector<std::array<double, 3>> &points,
										   const std::vector<std::array<double, 3>> &points_rgb,
										   const std::vector<std::array<double, 3>> &triangle_p1,
										   const std::vector<std::array<double, 3>> &triangle_p2, 
										   const std::vector<std::array<double, 3>> &triangle_p3, 
										   const std::vector<std::array<double, 3>> &triangle_rgb)
{
	Application::instance()->getNotifier()->addVisualizationAnnotationNode(visualizationModelID, name, UID, treeIcons, isHidden, edgeColorRGB, points, points_rgb,
												  triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
}

void Model::getFaceCurvatureRadius(const TopoDS_Shape *shape, std::list<double> &faceCurvatureRadius)
{
	// Loop through all faces of the entity and store the maximum curvatures in the list

	TopExp_Explorer exp;

	for (exp.Init(*shape, TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());
		double maxCurvature = GeometryOperations::getMaximumFaceCurvature(aFace);

		if (maxCurvature > 0.0) faceCurvatureRadius.push_back(1.0 / maxCurvature);
	}
}

void Model::entitiesSelected(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options)
{
	ot::JsonDocument doc;

	// Parse the document with the json string we have "received"
	doc.Parse(selectionInfo.c_str());

	// Check if the document is an object
	assert(doc.IsObject()); // Doc is not an object

	if (selectionAction == "PORT")
	{
		rapidjson::Value modelID = doc["modelID"].GetArray();
		ot::ConstJsonArray faceName = ot::json::getArray(doc, "faceName");

		std::list<EntityFaceAnnotationData> annotations;

		for (unsigned int i = 0; i < modelID.Size(); i++)
		{
			EntityFaceAnnotationData annotation;
			ot::UID entityID = modelID[i].GetUint64();
			std::string fname = faceName[i].GetString();

			EntityBase *entity = getEntityByID(entityID);
			assert(entity != nullptr);

			if (entity != nullptr)
			{
				annotation.setData(entity->getName(), entityID, fname);
				annotations.push_back(annotation);
			}
		}

		if (!annotations.empty())
		{
			int valueR = std::stoi(options["colorR"].c_str());
			int valueG = std::stoi(options["colorG"].c_str());
			int valueB = std::stoi(options["colorB"].c_str());

			createFaceAnnotation(annotations, valueR / 255.0, valueG / 255.0, valueB / 255.0, options["BaseName"]);
			modelChangeOperationCompleted(options["ModelStateName"]);
		}
	}
	else
	{
		assert(0); // Unknown type
	}
}

void Model::createFaceAnnotation(const std::list<EntityFaceAnnotationData> &annotations, double r, double g, double b, const std::string &baseName)
{		
	std::string annotationName;
	std::map<std::string, ot::UID> entityNameToIDMap = getEntityNameToIDMap();

	int count = 1;
	do
	{
		annotationName = baseName + std::to_string(count);
		count++;

	} while (entityNameToIDMap[annotationName]);

	EntityFaceAnnotation *annotationEntity = new EntityFaceAnnotation(0, nullptr, nullptr, nullptr);

	annotationEntity->setName(annotationName);
	annotationEntity->setColor(r, g, b);
	annotationEntity->setEditable(true);

	for (auto annotation : annotations)
	{
		annotationEntity->addFacePick(annotation);
	}

	GeometryOperations::EntityList allNewEntities;
	addEntityToModel(annotationEntity->getName(), annotationEntity, entityRoot, true, allNewEntities);

	updateAnnotationGeometry(annotationEntity);
}

void Model::updateAnnotationGeometry(EntityFaceAnnotation *annotationEntity)
{
	annotationEntity->renewFacets();

	EntityPropertiesInteger *faceCount = dynamic_cast<EntityPropertiesInteger*>(annotationEntity->getProperties().getProperty("Number of faces"));
	assert(faceCount != nullptr);

	for (int faceIndex = 1; faceIndex <= faceCount->getValue(); faceIndex++)
	{
		std::string index = "(" + std::to_string(faceIndex) + ")";

		// Now create a new visualization node (it may contain no nodes / triangles / edges if anything is invalid)

		EntityPropertiesString *geometryName = dynamic_cast<EntityPropertiesString*>(annotationEntity->getProperties().getProperty("Reference entity " + index));
		assert(geometryName != nullptr);

		ot::UID modelEntityID = getEntityNameToIDMap()[geometryName->getValue()];

		EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry *> (getEntityByID(modelEntityID));

		if (geometryEntity != nullptr)
		{
			// Now find the face in the geometry for the stored position
			std::list<TopoDS_Shape> faces;
			findFacesAtIndexFromShape(annotationEntity, faces, faceIndex, geometryEntity->getBrepEntity());

			for (auto face : faces)
			{
				// Now determine its bounding box 
				Bnd_Box Box;

				BRepBndLib::Add(face, Box);
				double xmin = 0.0, ymin = 0.0, zmin = 0.0, xmax = 0.0, ymax = 0.0, zmax = 0.0;
				Box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

				annotationEntity->getBoundingBox().extend(xmin, xmax, ymin, ymax, zmin, zmax);

				// Now facet the geometry and store the visualization item

				std::list<EntityBase *> entity;
				entity.push_back(geometryEntity);

				double deflection = calculateDeflectionFromListOfEntities(entity);

				std::string errors;
				GeometryOperations::facetEntity(face, nullptr, deflection, annotationEntity->getFacets()->getNodeVector(), annotationEntity->getFacets()->getTriangleList(), annotationEntity->getFacets()->getEdgeList(), annotationEntity->getFacets()->getFaceNameMap(), errors);
			}
		}
	}

	annotationEntity->storeUpdatedFacets();
}

void Model::findFacesAtIndexFromShape(EntityFaceAnnotation *annotationEntity, std::list<TopoDS_Shape> &facesList, int faceIndex, EntityBrep* brep)
{
	std::string index = "(" + std::to_string(faceIndex) + ")";

	EntityPropertiesString* faceName = dynamic_cast<EntityPropertiesString*>(annotationEntity->getProperties().getProperty("Face name " + index));

	TopExp_Explorer exp;
	for (exp.Init(brep->getBrep(), TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exp.Current());

		if (brep->getFaceName(aFace) == faceName->getValue())
		{
			facesList.push_back(exp.Current());
			return;
		}
	}
}

std::string Model::findMostRecentModelInStorage()
{
	std::map<std::string, bsoncxx::types::value> filterPairs;
	std::vector<std::string> columnNames;

	bsoncxx::types::b_utf8 bschemaType{ "Model" };
	bsoncxx::types::value schemaTypeVal(bschemaType);
	
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("SchemaType", schemaTypeVal));

	columnNames.push_back("Version");
	
	auto doc = bsoncxx::builder::basic::document{};
	if (!DataBase::instance().getAllDocumentsFromFilter(filterPairs, columnNames, doc))
	{
		return "";
	}

	auto docView = doc.view();
	bsoncxx::document::element ele = docView["Found"];

	long long maxVersion = 0;
	std::string objectID;

	if (ele && ele.type() == bsoncxx::type::k_array)
	{
		bsoncxx::array::view subarray{ ele.get_array().value };

		for (bsoncxx::array::element msg : subarray)
		{
			bsoncxx::document::view subdoc{ msg.get_document() };

			long long version = DataBase::getIntFromView(subdoc, "Version");

			if (version > maxVersion)
			{
				maxVersion = version;
				objectID   = subdoc["_id"].get_oid().value.to_string();
			}
		}
	}

	return objectID;
}

bool Model::GetDocumentFromEntityID(ot::UID entityID, bsoncxx::builder::basic::document &doc)
{
	ot::UID version = getStateManager()->getCurrentEntityVersion(entityID);

	return DataBase::instance().getDocumentFromEntityIDandVersion(entityID, version, doc);
}

EntityBase *Model::readEntityFromEntityIDandVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(entityID, version, doc))
	{
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase *entity = EntityFactory::instance().create(entityType);

	entity->restoreFromDataBase(parent, this, getStateManager(), doc_view, entityMap);

	return entity;
}

EntityBase *Model::readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap)
{
	ot::UID version = getStateManager()->getCurrentEntityVersion(entityID);

	return readEntityFromEntityIDandVersion(parent, entityID, version, entityMap);
}

void Model::projectOpen(const std::string& _customVersion)
{
	// First, clear the current content of the model
	clearAll();

	// Ensure that the database collection has a proper index
	try
	{
		DataBase::instance().createIndexIfNecessary();
	}
	catch (...)
	{
	}

	// Now we need to check whether the database is at the right schema and if not, we upgrade the database
	getStateManager()->checkAndUpgradeDataBaseSchema();

	// Load the model state for the latest version
	if (!getStateManager()->openProject(_customVersion))
	{
		// The model state could not be loaded -> the project is new
		resetToNew();
		modelChangeOperationCompleted("");

		// We still try to load the default settings template
		//TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();  // Already included in resetToNew();

		// Now setup the UI
		auto ui = Application::instance()->getUiComponent();
		if (ui)
		{
			enableQueuingHttpRequests(true);

			setupUIControls(ui);
			updateVersionGraph();

			enableQueuingHttpRequests(false);
		}

		m_isProjectOpen = true;
		return;
	}

	// Make sure that we do not have any unreferenced entities in the data base (e.g. left overy from a previously failed operation)
	getStateManager()->removeDanglingModelEntities();

	// Now get the model content from the storage. The model entity always has entity id 0. The model state manager has already 
	// read the current version of the model

	auto doc = bsoncxx::builder::basic::document{};
	
	if (!GetDocumentFromEntityID(0, doc))
	{
		OT_LOG_E("Failed to read most recent mode from storage. Resetting project to new...");
		displayMessage("ERROR: Unable to read most recent model from storage: " + projectName + "\n");
		resetToNew();
		modelChangeOperationCompleted("");

		// We still try to load the default settings template
		//TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();  // Already included in resetToNew()

		// Now setup the UI
		auto ui = Application::instance()->getUiComponent();
		if (ui)
		{
			enableQueuingHttpRequests(true);

			setupUIControls(ui);
			updateVersionGraph();

			enableQueuingHttpRequests(false);
		}

		m_isProjectOpen = true;
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	ot::UID schemaVersion = (ot::UID) DataBase::getIntFromView(doc_view, "SchemaVersion_Model");
	assert(schemaVersion == 2);

	long long entityRootId = doc_view["entityRoot"].get_int64();

	setModelStorageVersion((ot::UID) DataBase::getIntFromView(doc_view, "Version"));

	// Now we read the default template information
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();

	// Now setup the UI
	auto ui = Application::instance()->getUiComponent();
	if (ui && !uiCreated)
	{
		enableQueuingHttpRequests(true);
		
		setupUIControls(ui);

		enableQueuingHttpRequests(false);
	}

	updateVersionGraph();
	
	// Prefetch the topology entries which need to be read in order to build the model
	std::list<ot::UID> prefetchIds;
	getStateManager()->getListOfTopologyEntites(prefetchIds);

	prefetchDocumentsFromStorage(prefetchIds);

	// Now read the root entity (this will implicitly also recursively load all other subentities as well
	if (entityRootId != -1)
	{
		entityRoot = dynamic_cast<EntityContainer*>(readEntityFromEntityID(nullptr, entityRootId, entityMap));
	}

	// Process all entities and store the parameter values
	for (auto &entity : entityMap)
	{
		EntityParameter *parameter = dynamic_cast<EntityParameter *>(entity.second);
		if (parameter != nullptr)
		{
			setParameter(parameter->getName(), parameter->getNumericValue(), parameter);
		}
	}

	// Reset the modified flag
	resetModified();

	// reset the model data changed flag
	anyDataChangeSinceLastWrite = false;

	// update the undo information
	updateUndoRedoStatus();

	m_isProjectOpen = true;
}

void Model::updateVersionGraph()
{
	if (Application::instance()->isUiConnected() && !versionGraphCreated && visualizationModelID != 0)
	{
		enableQueuingHttpRequests(true);

		const ot::VersionGraphCfg& versionGraph = getStateManager()->getVersionGraph();
		sendVersionGraphToUI(versionGraph, getStateManager()->getModelStateVersion(), getStateManager()->getActiveBranch());

		enableQueuingHttpRequests(false);
	}
}

void Model::projectSave(const std::string &comment, bool silentlyCreateBranch)
{
	if (entityRoot != nullptr)
	{
		if (entityRoot->getModified()) setModified();
	}

	if (!getModified())
	{
		return;
	}

	// Enable write caching to database
	DataBase::instance().setWritingQueueEnabled(false);
	DataBase::instance().setWritingQueueEnabled(true);

	// Serialize the model content to the data base

	// First make sure that all entities are updated from their properties
	updateEntityProperties(true);

	// First recursively store the entities
	if (entityRoot != nullptr)
	{
		entityRoot->storeToDataBase();
	}

	// Now we store the specific information about the model itself

	if (anyDataChangeSinceLastWrite)
	{
		bool newModel = (getModelStorageVersion() == 0);
		assert(newModel); // We should have a single model entry only

		setModelStorageVersion(createEntityUID());

		if (newModel)
		{
			// This is the first time that we store the model
			getStateManager()->addNewEntity(0, 0, getModelStorageVersion(), ModelStateEntity::tEntityType::TOPOLOGY);
		}
		else
		{
			// The model has already been stored, so we need to update the version only
			getStateManager()->modifyEntityVersion(0, getModelStorageVersion());
		}

		auto doc = bsoncxx::builder::basic::document{};

		doc.append(bsoncxx::builder::basic::kvp("SchemaType", "Model"),
			bsoncxx::builder::basic::kvp("EntityID", (long long)0),
			bsoncxx::builder::basic::kvp("Version", (long long)getModelStorageVersion()),
			bsoncxx::builder::basic::kvp("SchemaVersion_Model", (long long)2),
			bsoncxx::builder::basic::kvp("ModelType", "Parametric3D"),
			bsoncxx::builder::basic::kvp("ActiveBranch", ""),
			bsoncxx::builder::basic::kvp("ActiveVersion", ""),
			bsoncxx::builder::basic::kvp("entityRoot", (long long)((entityRoot == nullptr) ? -1 : entityRoot->getEntityID()))
		);

		DataBase::instance().storeDataItem(doc);
		DataBase::instance().flushWritingQueue();

		anyDataChangeSinceLastWrite = false;
	}

	// Get the previous Model version
	std::string previousModelVersion = getStateManager()->getModelStateVersion();

	// Check whether we have redo information
	if (getStateManager()->canRedo() && !silentlyCreateBranch) {
		// Disable write caching to database (this will also flush all pending writes)
		DataBase::instance().setWritingQueueEnabled(false);

		Application::instance()->getNotifier()->promptChoice("There is redo information available which will be discarded if you change the model at this stage. \n\n"
			"Do you want to create a new version branch for these changes?", ot::MessageDialogCfg::Warning, ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No, "DiscardRedoInfoAndSave", comment);

		return;
	}

	// Save the model state for the latest version
	getStateManager()->saveModelState(false, false, comment);

	// Disable write caching to database (this will also flush all pending writes)
	DataBase::instance().setWritingQueueEnabled(false);

	// Add the new state and activate it
	std::string currentModelVersion = getStateManager()->getModelStateVersion();
	std::string activeBranch        = getStateManager()->getActiveBranch();

	if (currentModelVersion != previousModelVersion)  // In case no changed occurred, it may happen that the model state version did not change
	{
		ot::VersionGraphVersionCfg newVersion(currentModelVersion, "", comment);
		newVersion.setParentVersion(previousModelVersion);
		addNewVersionTreeStateAndActivate(activeBranch, newVersion);
	}

	updateUndoRedoStatus();
	resetModified();
}

void Model::promptResponse(const std::string& _type, ot::MessageDialogCfg::BasicButton _answer, const std::string& _parameter1)
{
	if (_type == "DiscardRedoInfoAndSave") {
		if (_answer != ot::MessageDialogCfg::Yes) {
			// We need to remove the redo information
			const std::list<std::string> removedStates = getStateManager()->removeRedoModelStates();

			if (!removedStates.empty()) {
				removeVersionGraphVersions(removedStates);
			}
		}

		projectSave(_parameter1, true);
	}
	else {
		OT_LOG_EAS("Unknown promt type \"" + _type + "\"");
	}
}

EntityBase* Model::getEntityByID(ot::UID _entityID) const
{
	auto entityBaseIt = entityMap.find(_entityID);
	if (entityBaseIt == entityMap.end())
	{
		assert(0); //Entity was not found
		return nullptr;
	}
	else
	{
		return entityBaseIt->second;
	}
}

void Model::enableQueuingHttpRequests(bool flag)
{
	Application::instance()->getNotifier()->enableQueuingHttpRequests(flag);
}

void Model::prefetchDocumentsFromStorage(std::list<ot::UID> &prefetchIds)
{
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;

	for (auto id : prefetchIds)
	{
		ot::UID entityID = id;
		ot::UID entityVersion = getStateManager()->getCurrentEntityVersion(entityID);
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	}

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);
}

void Model::setModified() 
{ 
	bool sendNotification = !isModified;

	isModified = true; 

	if (sendNotification && !shutdown)
	{
		Application::instance()->getNotifier()->isModified(visualizationModelID, true);
	}
}

void Model::resetModified()
{ 
	bool sendNotification = isModified;

	isModified = false; 

	if (sendNotification && !shutdown)
	{
		Application::instance()->getNotifier()->isModified(visualizationModelID, false);
	}
}

void Model::modelChangeOperationCompleted(const std::string &description, bool askForCreationOfBranch)
{
	projectSave(description, !askForCreationOfBranch);
}

void Model::uiIsAvailable(ot::components::UiComponent* _ui)
{
	enableQueuingHttpRequests(true);

	// Create the ui ribbon controls
	// Here we do not create the UI controls anymore, if the project is not open at this time. 
	// Therefore, the template UI configuration has not been read yet and the UI configuration is not yet
	// available. We will create the UI controls when we open the project.

	// setupUIControls();

	// Create the root visualization items 
	//createVisualizationItems();

	// The visualization items themselves will be created when the message setVisualizationModel is received, In case that the message
	// was already sent, we will create the visualization items now.
	if (visualizationModelID != 0)
	{
		if (entityRoot != nullptr)
		{
			entityRoot->addVisualizationNodes();
		}

		// Request a view reset
		Application::instance()->getNotifier()->resetAllViews(visualizationModelID);
	}

	updateVersionGraph();

	enableQueuingHttpRequests(false);
}

void Model::sendVersionGraphToUI(const ot::VersionGraphCfg& _versionGraph, const std::string& _currentVersion, std::string _activeBranch) {
	if (visualizationModelID != 0) {
		versionGraphCreated = true;

		// Add the first version to the active branch version so the graph can find the version

		ot::JsonDocument notify;
		notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_SetVersionGraph, notify.GetAllocator()), notify.GetAllocator());

		ot::JsonObject cfgObj;
		_versionGraph.addToJsonObject(cfgObj, notify.GetAllocator());

		notify.AddMember(OT_ACTION_PARAM_Config, cfgObj, notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_UI_GRAPH_ACTIVE, ot::JsonString(_currentVersion, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_UI_GRAPH_BRANCH, ot::JsonString(_activeBranch, notify.GetAllocator()), notify.GetAllocator());

		std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
		Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);

		setActiveVersionTreeState();
	}
}

void Model::setActiveVersionTreeState()
{
	const std::string& currentVersion = getStateManager()->getModelStateVersion();
	const std::string& activeBranch   = getStateManager()->getActiveBranch();

	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_SetVersionGraphActive, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_UI_GRAPH_ACTIVE, ot::JsonString(currentVersion, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_UI_GRAPH_BRANCH, ot::JsonString(activeBranch, notify.GetAllocator()), notify.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);
}

void Model::removeVersionGraphVersions(const std::list<std::string> &versions) {
	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RemoveVersionGraphVersions, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_ID, visualizationModelID, notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_List, ot::JsonArray(versions, notify.GetAllocator()), notify.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);
}

void Model::addNewVersionTreeStateAndActivate(const std::string& _branch, const ot::VersionGraphVersionCfg& _version) {
	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddAndActivateNewVersionGraphVersion, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_UI_GRAPH_BRANCH, ot::JsonString(_branch, notify.GetAllocator()), notify.GetAllocator());

	ot::JsonObject versionObj;
	_version.addToJsonObject(versionObj, notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_Config, versionObj, notify.GetAllocator());

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	Application::instance()->getNotifier()->queuedHttpRequestToUI(notify, prefetchIds);
}

void Model::sendMessageToViewer(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds) {
	// Here we need to add the information about the visualization model to the document 
	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, visualizationModelID, doc.GetAllocator());

	// And sent the message to the viewer
	Application::instance()->getNotifier()->queuedHttpRequestToUI(doc, prefetchIds);
}

void Model::requestConfigForModelDialog(const ot::UID& _entityID,const std::string _collectionType, const std::string& _targetFolder, const std::string& _elementType) {
	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_CreateConfig, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _entityID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(_collectionType, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(_targetFolder, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_ElementType, ot::JsonString(_elementType, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(Application::instance()->getUiComponent()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::instance().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::instance().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::instance().getDataBaseServerURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(Application::instance()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	Application::instance()->getLibraryManagementWrapper().requestCreateConfig(doc);
}

void Model::requestVisualisation(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg)
{
	Application::instance()->getVisualisationHandler().handleVisualisationRequest(_entityID, _visualisationCfg);
}

EntityBase *Model::findEntityFromName(const std::string &name)
{
	if (entityRoot == nullptr) return nullptr;

	return entityRoot->getEntityFromName(name);
}

std::string Model::getCurrentModelVersion()
{
	return getStateManager()->getModelStateVersion();
}

void Model::activateVersion(const std::string& _version)
{
	enableQueuingHttpRequests(true);

	if (getStateManager()->loadModelState(_version))
	{
		OT_LOG_D("Model state loaded { \"Version\": \"" + _version + "\" }");
		updateModelStateForUndoRedo();
	}

	// Refresh the view and send all messages
	refreshAllViews();

	enableQueuingHttpRequests(false);
}

void Model::versionSelected(const std::string& _version) {
	ot::PropertyGridCfg cfg;

	const ot::VersionGraphVersionCfg* version = getStateManager()->getVersionGraph().findVersion(_version);
	if (!version) {
		OT_LOG_EAS("Version not found \"" + _version + "\"");
		return;
	}

	ot::PropertyGroup* generalGroup = new ot::PropertyGroup("Version");
	cfg.addRootGroup(generalGroup);

	ot::PropertyString* nameProp = new ot::PropertyString("Name", version->getName(), ot::PropertyBase::IsReadOnly);
	ot::PropertyString* descriptionProp = new ot::PropertyString("Description", version->getDescription(), ot::PropertyBase::IsReadOnly);
	ot::PropertyString* labelProp = new ot::PropertyString("Label", version->getLabel());

	generalGroup->addProperty(nameProp);
	generalGroup->addProperty(descriptionProp);
	generalGroup->addProperty(labelProp);

	m_selectedVersion = _version;
	Application::instance()->getNotifier()->fillPropertyGrid(cfg);
}

void Model::versionDeselected() {
	m_selectedVersion.clear();
}

std::list<ot::UID> Model::getNewEntityIDs(unsigned long long count)
{
	std::list<ot::UID> newIDs;
	for (unsigned long long i = 1; i <= count; i++)
	{
		newIDs.push_back(createEntityUID());
	}

	return newIDs;
}

std::list<std::string> Model::getListOfFolderItems(const std::string &folder, bool recursive)
{
	std::list<std::string> folderItems;

	EntityBase *item = findEntityFromName(folder);
	if (item != nullptr)
	{
		EntityContainer *container = dynamic_cast<EntityContainer *>(item);
		if (container != nullptr)
		{
			for (auto child : container->getChildrenList())
			{
				folderItems.push_back(child->getName());
				if (recursive)
				{
					std::list<std::string> childrenList = getListOfFolderItems(child->getName(), recursive);
					folderItems.splice(folderItems.end(),childrenList);
				}
			}
		}
	}

	return folderItems;
}

void Model::getIDsOfFolderItemsOfType(EntityContainer *container, const std::string &className, bool recursive, std::list<ot::UID> &itemList)
{
	for (auto child : container->getChildrenList())
	{
		if (child->getClassName() == className)
		{
			itemList.push_back(child->getEntityID());
		}
		
		EntityContainer *itemContainer = dynamic_cast<EntityContainer *>(child);

		if (itemContainer != nullptr)
		{
			if (recursive)
			{
				getIDsOfFolderItemsOfType(itemContainer, className, recursive, itemList);
			}
		}
	}
}

std::list<ot::UID> Model::getIDsOfFolderItemsOfType(const std::string &folder, const std::string &className, bool recursive)
{
	std::list<ot::UID> folderItems;

	EntityBase *item = findEntityFromName(folder);
	if (item != nullptr)
	{
		EntityContainer *container = dynamic_cast<EntityContainer *>(item);
		if (container != nullptr)
		{
			getIDsOfFolderItemsOfType(container, className, recursive, folderItems);
		}
	}

	return folderItems;
}


void Model::addEntitiesToModel(const ot::NewModelStateInfo& _modelStateInfo, const std::string& _description, bool _saveModel, bool _askForCreationOfBranch, bool _considerVisualization) {
	addEntitiesToModel(
		_modelStateInfo.getTopologyEntityIDs(),
		_modelStateInfo.getTopologyEntityVersions(),
		_modelStateInfo.getTopologyForceVisible(),
		_modelStateInfo.getDataEntityIDs(),
		_modelStateInfo.getDataEntityVersions(),
		_modelStateInfo.getDataEntityParentIDs(),
		_description, _saveModel, _askForCreationOfBranch, _considerVisualization
	);
}

void Model::addEntitiesToModel(const std::list<ot::UID>& _topologyEntityIDList, const std::list<ot::UID>& _topologyEntityVersionList, const std::list<bool>& _topologyEntityForceVisible,
	const std::list<ot::UID>& _dataEntityIDList, const std::list<ot::UID>& _dataEntityVersionList, const std::list<ot::UID>& _dataEntityParentList, 
	const std::string& _description, bool _saveModel, bool _askForCreationOfBranch, bool _considerVisualization)
{
	enableQueuingHttpRequests(true);

	OTAssert(_topologyEntityIDList.size() == _topologyEntityVersionList.size(), "Topology ID and Version size mismatch");
	OTAssert(_topologyEntityIDList.size() == _topologyEntityForceVisible.size(), "Topology ID and Force Visible size mismatch");
	OTAssert(_dataEntityIDList.size() == _dataEntityVersionList.size(), "Data ID and Version size mismatch");
	OTAssert(_dataEntityIDList.size() == _dataEntityParentList.size(), "Data ID and Parent size mismatch");

	// First of all, we deal with all the data entities and add them to the model state

	auto entityVersion = _dataEntityVersionList.begin();
	auto parentID = _dataEntityParentList.begin();

	for (ot::UID entityID : _dataEntityIDList)
	{
		getStateManager()->storeEntity(entityID, *parentID, *entityVersion, ModelStateEntity::tEntityType::DATA);
		setModified();
		entityVersion++;
		parentID++;
	}

	// In a second step, we deal with all the topology entities. We need to load these entities here and add them to the model.
	// If the same entity already exists, the old one needs to be deleted and replaced by the new one.

	// Prefetch all entities for faster loading
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
	auto version = _topologyEntityVersionList.begin();
	for (ot::UID id : _topologyEntityIDList)
	{
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(id, *version));
		version++;
	}

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);

	std::list<EntityBase *> entityList;
	std::list<ot::UID> removeFromDisplay;

	version = _topologyEntityVersionList.begin();

	for (ot::UID id : _topologyEntityIDList)
	{
		EntityBase *entity = readEntityFromEntityIDandVersion(nullptr, id, *version, entityMap);
		assert(entity != nullptr);
		assert(entity->getEntityType() == EntityBase::TOPOLOGY);

		entityList.push_back(entity);

		// Now check whether the entity already exists and if so, delete it and mark if for removal from the display
		EntityBase *oldEntity = findEntityFromName(entity->getName());
		if (oldEntity != nullptr)
		{
			removeFromDisplay.push_back(oldEntity->getEntityID());

			// Remove the entity from the entity map and also from the model state
			removeEntityFromMap(oldEntity, false, false);
			delete oldEntity;
		}

		version++;
	}
	std::set<std::string> newEntitiesNames;
	for (EntityBase* newEntity : entityList)
	{
		const std::string entityName = newEntity->getName();
		if (newEntitiesNames.find(entityName) == newEntitiesNames.end())
		{
			newEntitiesNames.insert(entityName);
		}
		else
		{
			OT_LOG_E("Tried to store an entity with the same name twice. Entity name: " + entityName);
			assert(0);
			return;
		}
	}

	// Remove the "old" elements from the display
	if (!removeFromDisplay.empty() && _considerVisualization)
	{
		removeShapesFromVisualization(removeFromDisplay);
	}

	addTopologyEntitiesToModel(entityList, _topologyEntityForceVisible, _considerVisualization);

	// Finally refresh the views and save the new model state
	
	refreshAllViews();
	enableQueuingHttpRequests(false);

	if (_saveModel)
	{
		modelChangeOperationCompleted(_description, _askForCreationOfBranch);
	}
}

void Model::getListOfAllChildEntities(EntityBase* entity, std::list<std::pair<ot::UID, ot::UID>>& childrenEntities)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(entity);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			childrenEntities.push_back(std::pair<ot::UID, ot::UID>(child->getEntityID(), child->getEntityStorageVersion()));

			getListOfAllChildEntities(child, childrenEntities);
		}
	}
}

void Model::addChildrenEntitiesToList(EntityGeometry *entity, std::list<std::pair<ot::UID, ot::UID>> &childrenEntities)
{
	childrenEntities.push_back(std::pair<ot::UID, ot::UID>(entity->getEntityID(), entity->getEntityStorageVersion()));

	for (auto child : entity->getChildrenList())
	{
		EntityGeometry *geomChild = dynamic_cast<EntityGeometry*>(child);
		addChildrenEntitiesToList(geomChild, childrenEntities);
	}
}

void Model::recursiveReplaceEntityName(EntityBase *entity, const std::string &oldName, const std::string &newName, std::list<EntityBase*> &entityList)
{
	// Here we replace the part "oldName" in the name string by the "newName"
	std::string name = entity->getName();
	
	assert(name.size() >= oldName.size());
	assert(name.substr(0, oldName.size()) == oldName);

	name = name.substr(oldName.size());
	name = newName + name;

	entity->setName(name);

	entityList.push_back(entity);

	EntityContainer *container = dynamic_cast<EntityContainer*>(entity);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			recursiveReplaceEntityName(child, oldName, newName, entityList);
		}
	}
}

void Model::recursivelyAddEntityInfo(EntityBase *entity, std::map<ot::UID, ot::UID> &entityVersionMap)
{
	assert(entity != nullptr);

	EntityGeometry *geometry = dynamic_cast<EntityGeometry *>(entity);
	if (geometry != nullptr)
	{
		entityVersionMap[geometry->getBrepStorageObjectID()]    = getStateManager()->getCurrentEntityVersion(geometry->getBrepStorageObjectID());
		entityVersionMap[geometry->getFacetsStorageObjectID()] = getStateManager()->getCurrentEntityVersion(geometry->getFacetsStorageObjectID());

		for (auto child : geometry->getChildrenList())
		{
			recursivelyAddEntityInfo(child, entityVersionMap);
		}
	}
}

void Model::addGeometryOperation(ot::UID geomEntityID, ot::UID geomEntityVersion, const std::string &geomEntityName,
							     std::list<ot::UID> &dataEntityIDList, std::list<ot::UID> &dataEntityVersionList, std::list<ot::UID> &dataEntityParentList, std::list<std::string> &childrenList,
								 const std::string &description)
{
	enableQueuingHttpRequests(true);

	// Get list of children entities
	std::list<EntityBase *> childrenEntityList;
	std::map<std::string, EntityBase *> childrenNameMap;
	std::map<ot::UID, ot::UID> entityVersionMap;
	for (auto childName : childrenList)
	{
		EntityBase *entity = findEntityFromName(childName);

		if (entity != nullptr)
		{
			childrenNameMap[childName] = entity;
			childrenEntityList.push_back(entity);

			recursivelyAddEntityInfo(entity, entityVersionMap);
		}
	}

	// Remove children entities from model, display, statemanager 
	std::list<ot::UID> removeFromDisplay;

	for (auto entity : childrenEntityList)
	{
		removeFromDisplay.push_back(entity->getEntityID());

		// Remove the entity from the entity map and also from the model state
		std::list<EntityBase*> removedEntityList;
		removeEntityWithChildrenFromMap(entity, false, true, removedEntityList);  // In this operation, we are removing the entity from the model. However, it will be added back again
																				  // in a later step, so the dependecy of the entityID on the parameter remains. Therefore, we keep it here.
		
		if (entity->getParent() != nullptr)
		{
			entity->getParent()->removeChild(entity);
			entity->setParent(nullptr);
		}
	}

	removeShapesFromVisualization(removeFromDisplay);

	// Load the new topology entity, get its name and add it to the model (using addEntitiesToModel function)
	std::list<ot::UID> topologyEntityIDList = {geomEntityID};
	std::list<ot::UID> topologyEntityVersionList = {geomEntityVersion};
	std::list<bool> topologyEntityForceVisible = {false};
	addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, description, false, true, true);

	// for all children, change the name and the names of the children

	std::list<EntityBase*> allChildrenList;
	bool isBaseShape = true;
	for (auto childName : childrenList)
	{
		assert(childrenNameMap.count(childName) > 0);

		// Remove all path from the child
		size_t index = childName.rfind('/');

		std::string childNameWithoutPath = childName.substr(index+1);

		if (isBaseShape)
		{
			EntityGeometry *geomEntity = dynamic_cast<EntityGeometry *>(childrenNameMap[childName]);
			if (geomEntity != nullptr)
			{
				std::string iconVisible, iconHidden;
				geomEntity->getOldTreeIcons(iconVisible, iconHidden);

				iconVisible += "Base";
				iconHidden += "Base";

				geomEntity->setOldTreeIcons(iconVisible, iconHidden);
			}

			isBaseShape = false;
		}

		// add the new children path
		std::string newChildName = geomEntityName + "/" + childNameWithoutPath;

		recursiveReplaceEntityName(childrenNameMap[childName], childName, newChildName, allChildrenList);
	}

	// Add the children entities to the model at the new position (including the children)
	for (auto entity : allChildrenList)
	{
		// Since allChildrenList contains all children, we need to insert the entities one by one. This implies that
		// all hierarchical functionality on the entity needs to be turned off. Therefore, we need to detach the 
		// entity from its parent and also remove all pointers to the children. This information will automatically be
		// rebuilt by the addEntityToModel operation.
		entity->detachFromHierarchy();

		// The children should no longer control the parent visibility
		entity->setSelectChildren(false);
		entity->setManageChildVisibility(false);
		entity->setManageParentVisibility(false);

		// Next, add the entity to the model again (at a new position)
		std::list<EntityBase*> newEntities;
		addEntityToModel(entity->getName(), entity, entityRoot, true, newEntities);

		// Register the data entities in the model state
		EntityGeometry *geometry = dynamic_cast<EntityGeometry *>(entity);
		if (geometry != nullptr)
		{
			geometry->setShowWhenSelected(true);

			getStateManager()->storeEntity(geometry->getBrepStorageObjectID(), entity->getEntityID(), entityVersionMap[geometry->getBrepStorageObjectID()], ModelStateEntity::tEntityType::DATA);
			getStateManager()->storeEntity(geometry->getFacetsStorageObjectID(), entity->getEntityID(), entityVersionMap[geometry->getFacetsStorageObjectID()], ModelStateEntity::tEntityType::DATA);
		}

		// Finally add the entity to the visualization
		entity->setInitiallyHidden(true);
		entity->addVisualizationNodes();
	}

	// refresh the view and save the model
	Application::instance()->getNotifier()->selectObject(visualizationModelID, geomEntityID);
	refreshAllViews();
	enableQueuingHttpRequests(false);

	modelChangeOperationCompleted(description);
}

void Model::deleteEntitiesFromModel(const ot::UIDList& _entityIDList, bool _saveModel) {
	std::list<EntityBase*> entityList;
	for (ot::UID id : _entityIDList) {
		auto it = entityMap.find(id);
		if (it != entityMap.end()) {
			entityList.push_back(it->second);
		}
	}

	deleteEntitiesFromModel(entityList, _saveModel);
}

void Model::deleteEntitiesFromModel(const std::list<std::string>& _entityNameList, bool _saveModel)
{
	std::list<EntityBase *> entityList;
	for (const std::string& name : _entityNameList)
	{
		EntityBase *entity = findEntityFromName(name);
		
		if (entity != nullptr)
		{
			entityList.push_back(entity);
		}
	}

	deleteEntitiesFromModel(entityList, _saveModel);
}

void Model::deleteEntitiesFromModel(const std::list<EntityBase*>& _entityList, bool _saveModel) {
	if (_entityList.empty()) {
		return;
	}

	// Remove all children from the list
	std::list<EntityBase*> topLevelEntities = removeChildrenFromList(_entityList);

	// Now we remove all entities from the visualizaton and delete them afterward 
	std::list<ot::UID> removeFromDisplay;

	for (EntityBase* entity : topLevelEntities) {
		removeFromDisplay.push_back(entity->getEntityID());

		// Remove the entity from the entity map and also from the model state
		removeEntityFromMap(entity, false, false);

		delete entity;
	}

	removeShapesFromVisualization(removeFromDisplay);

	setModified();

	if (_saveModel) {
		modelChangeOperationCompleted("delete objects");
	}
}

void Model::setMeshingActive(ot::UID meshEntityID, bool flag)
{
	meshingActive[meshEntityID] = flag;
}

bool Model::getMeshingActive(ot::UID meshEntityID)
{
	if (meshingActive.count(meshEntityID) == 0) return false;

	return meshingActive[meshEntityID];
}

void Model::updateVisualizationEntity(ot::UID visEntityID, ot::UID visEntityVersion, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion)
{
	// We add the binaryData items as childs to the visualization entity id and update this entity. This function is called as part of the 
	// update process, so the modelChangeOperationCompleted function will be called later.

	EntityVis2D3D *visEntity = dynamic_cast<EntityVis2D3D *>(getEntityByID(visEntityID));
	assert(visEntity != nullptr);
	if (visEntity == nullptr) return;

	ot::UID oldDataItemID = visEntity->getDataID();
	ot::UID oldDataItemVersion = visEntity->getDataVersion();

	if (oldDataItemID > 0)
	{
		getStateManager()->removeEntity(oldDataItemID);

		std::list<std::pair<ot::UID, ot::UID>> deleteDocuments;
		deleteDocuments.push_back(std::pair<ot::UID, ot::UID>(oldDataItemID, oldDataItemVersion));

		// Now we remove the old display entity to avoid unnecessary growth in data base
		DataBase::instance().deleteDocuments(deleteDocuments);
	}

	visEntity->setDataID(binaryDataItemID);
	visEntity->setDataVersion(binaryDataItemVersion);

	if (binaryDataItemID > 0)
	{
		getStateManager()->addNewEntity(binaryDataItemID, visEntity->getEntityID(), binaryDataItemVersion, ModelStateEntity::tEntityType::DATA);
	}

	std::list<std::pair<ot::UID, ot::UID>> prefetchIds;
	prefetchIds.push_back(std::pair<ot::UID, ot::UID>(visEntity->getEntityID(), visEntityVersion));

	visEntity->storeToDataBase();

	// Now send the update view message to the viewer
	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_UpdateVis2D3DNode, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityID, visEntity->getEntityID(), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(DataBase::instance().getCollectionName(), notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_DataID, binaryDataItemID, notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_DataVersion, binaryDataItemVersion, notify.GetAllocator());

	sendMessageToViewer(notify, prefetchIds);
}

void Model::updateGeometryEntity(ot::UID geomEntityID, ot::UID brepEntityID, ot::UID brepEntityVersion, ot::UID facetsEntityID, ot::UID facetsEntityVersion, bool overrideGeometry, const ot::PropertyGridCfg& _configuration, bool updateProperties)
{
	EntityGeometry *geomEntity = dynamic_cast<EntityGeometry *>(getEntityByID(geomEntityID));
	if (geomEntity == nullptr)
	{
		assert(0); // The geometry entity does not exist
		return;
	}

	// Update the properties, if requested
	if (updateProperties)
	{
		geomEntity->getProperties().buildFromConfiguration(_configuration, getRootNode());
	}

	// Release the brep and facets (if loaded)
	geomEntity->releaseBrep();
	geomEntity->releaseFacets();

	if (overrideGeometry)
	{
		// Delete the previous (temporary) geometry entity from the data base
		std::list<std::pair<ot::UID, ot::UID>> deleteDocuments;
		deleteDocuments.push_back(std::pair<ot::UID, ot::UID>(geomEntity->getEntityID(), geomEntity->getEntityStorageVersion()));
		DataBase::instance().deleteDocuments(deleteDocuments);

		//geomEntity->setEntityStorageVersion(geomEntity->getEntityStorageVersion() - 1);
	}

	// Now remove the previous brep and facets from the model state
	getStateManager()->removeEntity(geomEntity->getBrepStorageObjectID());
	getStateManager()->removeEntity(geomEntity->getFacetsStorageObjectID());

	// Add the new brep and facets to the model state
	getStateManager()->addNewEntity(brepEntityID, geomEntityID, brepEntityVersion, ModelStateEntity::DATA);
	getStateManager()->addNewEntity(facetsEntityID, geomEntityID, facetsEntityVersion, ModelStateEntity::DATA);

	// Update the data in the geom entity
	geomEntity->replaceBrepStorageID(brepEntityID);
	geomEntity->replaceFacetsStorageID(facetsEntityID);

	// Reset the properties needs update flags
	geomEntity->getProperties().forceResetUpdateForAllProperties();

	// Finally save the new geom entity state
	geomEntity->storeToDataBase();

	// Update the display
	geomEntity->addVisualizationNodes();
}

void Model::updateTopologyEntities(const ot::NewModelStateInfo& _modelStateInfo, const std::string& _comment, bool _considerVisualization) {
	updateTopologyEntities(_modelStateInfo.getTopologyEntityIDs(), _modelStateInfo.getTopologyEntityVersions(), _comment, _considerVisualization);
}

void Model::updateTopologyEntities(const ot::UIDList& _topoEntityID, const ot::UIDList& _topoEntityVersion, const std::string& _comment, bool _considerVisualization)
{
	enableQueuingHttpRequests(true);

	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
	auto version = _topoEntityVersion.begin();
	for (auto id : _topoEntityID)
	{
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(id, *version));
		version++;
	}
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);

	auto topoEntityVersion = _topoEntityVersion.begin();
	std::list<ot::UID> removeFromDisplay;
	std::list<EntityBase*> entityList;
	std::list<bool> topologyEntityForceVisible;
	const bool considerDependingDataEntities = false;
	for (ot::UID topoEntityID : _topoEntityID)
	{
		std::map<ot::UID, EntityBase*> map;
		EntityBase* newEntity = readEntityFromEntityIDandVersion(nullptr, topoEntityID, *topoEntityVersion, map);
		topoEntityVersion++;
		EntityBase* oldEntity = findEntityFromName(newEntity->getName());

		if (oldEntity != nullptr)
		{
			removeFromDisplay.push_back(oldEntity->getEntityID());

			//If we have a container entity, we need to adjust the parent/child relationship here in the ModelService. This may not deal with all data entities.
			EntityContainer*  oldContainer =	dynamic_cast<EntityContainer*>(oldEntity);
			if (oldContainer != nullptr)
			{
				EntityContainer*  newContainer =	dynamic_cast<EntityContainer*>(newEntity);
				assert(newContainer != nullptr);
				for (EntityBase* child : oldContainer->getChildrenList())
				{
					newContainer->addChild(child);
					child->setParent(newContainer);
				}

				for (EntityBase* child : newContainer->getChildrenList())
				{
					oldContainer->removeChild(child);
				}
			}

			//First we remove possible data entities from the entity map that have the old entity as their parent.
			//ot::UIDList dataChildren;
			for (auto entityByID : entityMap)
			{
				EntityBase* entity = entityByID.second;
				if (entity->getEntityType() == EntityBase::DATA)
				{
					if (entity->getParent() == oldEntity)
					{
						entity->setParent(newEntity);
						//dataChildren.push_back(entity->getEntityID());
					}
				}
			}
			/*for (ot::UID dataChild : dataChildren)
			{
				entityMap.erase(dataChild);
			}*/
			
			// Remove the topo entity from the entity map and also from the model state
			removeEntityFromMap(oldEntity, false, false, considerDependingDataEntities);

			

			delete oldEntity;

			entityList.push_back(newEntity);
			topologyEntityForceVisible.push_back(false);
		}
		else
		{
			//Was actually a new entity. HEre is no special handling necessary.
			entityList.push_back(newEntity);
			topologyEntityForceVisible.push_back(false);
		}
	}

	if (!removeFromDisplay.empty() && _considerVisualization)
	{
		removeShapesFromVisualization(removeFromDisplay);
	}

	//Now we can add the new version of the topology entities to the model state
	addTopologyEntitiesToModel(entityList, topologyEntityForceVisible, _considerVisualization);


	refreshAllViews();
	enableQueuingHttpRequests(false);
	modelChangeOperationCompleted(_comment);
}

void Model::requestUpdateVisualizationEntity(ot::UID visEntityID)
{
	// We add the binaryData items as childs to the visualization entity id and update this entity. This function is called as part of the 
	// update process, so the modelChangeOperationCompleted function will be called later.

	EntityVis2D3D *visEntity = dynamic_cast<EntityVis2D3D *>(getEntityByID(visEntityID));
	if (visEntity == nullptr)
	{
		assert(0); // Unknown type
		return;
	}
	assert(visEntity != nullptr);
	
	std::list<ot::UID> entityIDs;
	std::list<ot::UID> entityVersions;
	std::list<ot::UID> brepVersions;

	entityIDs.push_back(visEntity->getEntityID());
	entityVersions.push_back(visEntity->getEntityStorageVersion());
	brepVersions.push_back(0);

	std::thread workerThread(&Model::performUpdateVisualizationEntity, this, entityIDs, entityVersions, brepVersions, visEntity->getServicesForCallback(EntityBase::Callback::DataNotify));
	workerThread.detach();
}

void Model::performUpdateVisualizationEntity(std::list<ot::UID> entityIDs, std::list<ot::UID> entityVersions, std::list<ot::UID> brepVersions, std::list<std::string> _callbackServices)
{
	ot::LockTypes lockFlag(ot::LockType::ModelWrite | ot::LockType::NavigationWrite | ot::LockType::ViewWrite | ot::LockType::Properties);
	ot::UILockWrapper uiLock(Application::instance()->getUiComponent(), lockFlag);

	ot::JsonDocument notify;
	ot::JsonArray changedEntitiesInfos;
	for (ot::UID& entityID : entityIDs)
	{
		auto changedEntityByID = entityMap.find(entityID);
		assert(changedEntityByID != entityMap.end());
		ot::EntityInformation entityInfo(changedEntityByID->second);
		ot::JsonObject entityInfoSerialised;
		entityInfo.addToJsonObject(entityInfoSerialised, notify.GetAllocator());
		changedEntitiesInfos.PushBack(entityInfoSerialised, notify.GetAllocator());
	}

	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_PropertyChanged, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityInfo, changedEntitiesInfos, notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDs, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersions, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_BrepVersionList, ot::JsonArray(brepVersions, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, true, notify.GetAllocator());

	for (const std::string& owningService : _callbackServices) {
		Application::instance()->getNotifier()->sendMessageToService(false, owningService, notify);
	}
}

void Model::getEntityVersions(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersions)
{
	entityVersions.clear();

	for (auto entity : entityIDList)
	{
		entityVersions.push_back(getStateManager()->getCurrentEntityVersion(entity));
	}
}

void Model::getEntityNames(std::list<ot::UID> &entityIDList, std::list<std::string> &entityNames)
{
	entityNames.clear();

	for (auto entity : entityIDList)
	{
		EntityBase *baseEntity = getEntityByID(entity);

		if (baseEntity != nullptr)
		{
			entityNames.push_back(baseEntity->getName());
		}
		else
		{
			entityNames.push_back(""); // Unknown name
		}
	}
}

void Model::getEntityTypes(std::list<ot::UID> &entityIDList, std::list<std::string> &entityTypes)
{
	entityTypes.clear();

	for (auto entity : entityIDList)
	{
		EntityBase *baseEntity = getEntityByID(entity);

		if (baseEntity != nullptr)
		{
			entityTypes.push_back(baseEntity->getClassName());
		}
		else
		{
			entityTypes.push_back(""); // Unknown type
		}
	}
}

std::list<ot::UID> Model::getAllGeometryEntitiesForMeshing()
{
	std::list<EntityGeometry *> geometryEntities;
	getAllGeometryEntities(geometryEntities);

	std::list<ot::UID> meshingEntities;

	for (auto entity : geometryEntities)
	{
		EntityPropertiesBoolean *considerForMeshing = dynamic_cast<EntityPropertiesBoolean*>(entity->getProperties().getProperty("Consider for meshing"));

		if (considerForMeshing != nullptr)
		{
			if (considerForMeshing->getValue())
			{
				meshingEntities.push_back(entity->getEntityID());
			}
		}
		else
		{
			meshingEntities.push_back(entity->getEntityID());
		}
	}

	return meshingEntities;
}

void Model::handleUndoLastOperation()
{
	enableQueuingHttpRequests(true);

	if (getStateManager()->undoLastOperation())
	{
		updateModelStateForUndoRedo();
	}
	//else
	//{
	//	// remove all entities except for the templates
	//	//removeAllNonTemplateEntities();

	//	// No undo information available -> reset the project to an empty project
	//	resetToNew();
	//	modelChangeOperationCompleted("");

	//	resetModified();
	//}

	// Refresh the view and send all messages
	refreshAllViews();

	enableQueuingHttpRequests(false);
}

void Model::handleRedoNextOperation()
{
	enableQueuingHttpRequests(true);

	if (getStateManager()->redoNextOperation())
	{
		updateModelStateForUndoRedo();
	}

	// Refresh the view and send all messages
	refreshAllViews();

	enableQueuingHttpRequests(false);
}

void Model::removeAllNonTemplateEntities()
{
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::instance().getCollectionName());

	auto deleteDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << bsoncxx::builder::stream::open_document << "$ne" << "DefaultTemplate"
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	docBase.DeleteDocuments(std::move(deleteDoc));
}

void Model::updateModelStateForUndoRedo()
{
	// Loop through all entities of the current model and check whether they are part of the model state and up-to-date
	std::list<ot::UID> removeFromDisplay;

	std::map<ot::UID, EntityBase *> entityMapTmp = entityMap;

	for (auto entity : entityMapTmp)
	{
		// The item might have already been deleted as a child of a previously deleted item. Therefore, we need to check
		// here if the item is still in the map before deleting it.

		if (entityMap.count(entity.first) > 0)
		{
			long long version = getStateManager()->getCurrentEntityVersion(entity.first);
			if (version == -1 || version != entity.second->getEntityStorageVersion())
			{
				// The entity is not part of the current model state or not at the right version -> delete the entity

				// Check whether we are deleting a paramter. If so, remove it from the list
				if (dynamic_cast<EntityParameter *>(entity.second) != nullptr)
				{
					removeParameter(entity.second->getName());
				}

				if (entity.second == entityRoot)
				{
					// We are deleting the top level entity
					for (auto child : entityRoot->getChildrenList())
					{
						removeFromDisplay.push_back(child->getEntityID());
					}

					delete entity.second;
					entityRoot = nullptr;
				}
				else
				{
					removeFromDisplay.push_back(entity.second->getEntityID());

					removeEntityFromMap(entity.second, true, true);

					delete entity.second;
				}
			}
		}
	}		
	
	Application::instance()->getNotifier()->setTreeStateRecording(visualizationModelID, true);

	removeShapesFromVisualization(removeFromDisplay);

	if (entityMap.empty())
	{
		// We need to reload the entire project
		projectOpen();

		//createVisualizationItems();

		entityRoot->addVisualizationNodes();
	}
	else
	{
		// Load all entities which are new in the model state and not part of the model yet
		std::list<ot::UID> topologyEntitiesLeft;
		getStateManager()->getListOfTopologyEntites(topologyEntitiesLeft);

		// Now we check, which of the entities has not been loaded yet
		std::list<ot::UID> prefetchIds;
		for (auto entity : topologyEntitiesLeft)
		{
			if (entityMap.count(entity) == 0)
			{
				// This entity is currenty not loaded
				prefetchIds.push_back(entity);
			}
		}

		// Prefetch all entities which are needed below
		prefetchDocumentsFromStorage(prefetchIds);

		if (entityMap.empty()) {
			OT_LOG_EA("Entity map is empty after initialization");
		}

		do
		{
			std::list<ot::UID> topologyEntities = topologyEntitiesLeft;
			topologyEntitiesLeft.clear();

			for (auto entity : topologyEntities)
			{
				if (entity != 0 && entityMap.count(entity) == 0)
				{
					// This entity needs to be loaded

					ot::UID parentID = getStateManager()->getCurrentEntityParent(entity);
					if (parentID != 0 && getStateManager()->getCurrentEntityVersion(parentID) != -1)
					{
						if (entityMap.count(parentID) == 0)
						{
							// The parent of the current entity has not been loaded yet -> we need to repeat once the parent is loaded
							topologyEntitiesLeft.push_back(entity);
						}
						else
						{
							EntityBase *parentEntity = getEntityByID(parentID);

							if (entityMap.empty()) {
								OT_LOG_EA("Entity map is empty before reading entity");
							}

							EntityBase *newEntity = readEntityFromEntityID(parentEntity, entity, entityMap);
							newEntity->addVisualizationNodes();

							if (dynamic_cast<EntityContainer*>(parentEntity) != nullptr)
							{
								dynamic_cast<EntityContainer*>(parentEntity)->addChild(newEntity);
							}

							// Check whether we are adding a parameter. If so, add it to the map
							if (dynamic_cast<EntityParameter*>(newEntity) != nullptr)
							{
								EntityParameter *parameter = dynamic_cast<EntityParameter*>(newEntity);
								setParameter(parameter->getName(), parameter->getNumericValue(), parameter);
							}

							if (entityMap.empty()) {
								OT_LOG_EA("Entity map is empty after reading entity");
							}
						}
					}
					else
					{
						if (parentID == 0) {
							// This should not happen
							OT_LOG_EA("The top level ID affected by an undo operation");
						}
					}
				}
			}
		} while (!topologyEntitiesLeft.empty());
	}
	// Now reset the modified flag for all entities and the model itself
	for (auto entity : entityMap)
	{
		entity.second->resetModified();
	}

	resetModified();
	updateUndoRedoStatus();
	setActiveVersionTreeState();

	Application::instance()->getNotifier()->setTreeStateRecording(visualizationModelID, false);
}


void Model::hideEntities(std::list<ot::UID> &hiddenEntityIDs)
{
	if (visualizationModelID != 0)
	{
		Application::instance()->getNotifier()->hideEntities(visualizationModelID, hiddenEntityIDs);
	}
}

void Model::loadDefaultMaterials()
{
	std::string error = TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultMaterials();

	if (!error.empty())
	{
		Application::instance()->getNotifier()->displayMessage(error);
	}

	for (auto material : TemplateDefaultManager::getTemplateDefaultManager()->getDefaultMaterials())
	{
		std::string materialName = getMaterialRootName() + "/" + material.first;

		
		EntityMaterial *materialItem =  MaterialHandler::createNewMaterial(materialName);

		for (auto prop : material.second)
		{
			std::string propertyName = prop.first;
			DefaultValue propertyValue = prop.second;

			EntityPropertiesBase *prop = materialItem->getProperties().getProperty(propertyName);

			if (prop == nullptr)
			{
				Application::instance()->getNotifier()->displayMessage("ERROR: Invalid property for material:" + materialName + ", property: " + propertyName + "\n");
				continue;
			}
		
			switch (propertyValue.type)
			{
			case DefaultValue::BOOL:
			{
				EntityPropertiesBoolean *boolProp = dynamic_cast<EntityPropertiesBoolean *>(prop);
				if (boolProp == nullptr)
				{
					Application::instance()->getNotifier()->displayMessage("ERROR: Incorrect data type for material:" + materialName + ", property: " + propertyName + "\n");
				}
				else
				{
					boolProp->setValue(propertyValue.b);
				}
			}
			break;
			case DefaultValue::DOUBLE:
			{
				EntityPropertiesDouble *doubleProp = dynamic_cast<EntityPropertiesDouble *>(prop);
				if (doubleProp == nullptr)
				{
					Application::instance()->getNotifier()->displayMessage("ERROR: Incorrect data type for material:" + materialName + ", property: " + propertyName + "\n");
				}
				else
				{
					doubleProp->setValue(propertyValue.d);
				}
			}
			break;
			case DefaultValue::STRING:
			{
				EntityPropertiesString *stringProp = dynamic_cast<EntityPropertiesString *>(prop);
				if (stringProp == nullptr)
				{
					EntityPropertiesSelection *selProp = dynamic_cast<EntityPropertiesSelection *>(prop);

					if (selProp == nullptr)
					{
						Application::instance()->getNotifier()->displayMessage("ERROR: Incorrect data type for material:" + materialName + ", property: " + propertyName + "\n");
					}
					else
					{
						selProp->setValue(propertyValue.s);
					}
				}
				else
				{
					stringProp->setValue(propertyValue.s);
				}
			}
			break;
			case DefaultValue::LONG:
			{
				EntityPropertiesInteger *intProp = dynamic_cast<EntityPropertiesInteger *>(prop);
				if (intProp == nullptr)
				{
					Application::instance()->getNotifier()->displayMessage("ERROR: Incorrect data type for material:" + materialName + ", property: " + propertyName + "\n");
				}
				else
				{
					intProp->setValue(propertyValue.l);
				}
			}
			break;
			case DefaultValue::COLOR:
			{
				EntityPropertiesColor *colorProp = dynamic_cast<EntityPropertiesColor *>(prop);
				if (colorProp == nullptr)
				{
					Application::instance()->getNotifier()->displayMessage("ERROR: Incorrect data type for material:" + materialName + ", property: " + propertyName + "\n");
				}
				else
				{
					colorProp->setColorRGB(propertyValue.c[0], propertyValue.c[1], propertyValue.c[2]);
				}
			}
			break;
			default:
				assert(0); // Unknown property type
			}
		}
	}
}

std::string Model::checkParentUpdates(std::list<ot::UID> &modifiedEntities)
{
	std::list<ot::UID> affectedParentList;
	std::map<ot::UID, bool> affectedParentMap;

	// First, we build a unique list of all parent geometry entities which need to be updated.
	// Here we consider only one level up, since we will then update their parents in a second round.
	for (auto entityID : modifiedEntities)
	{
		EntityBase *entity = getEntityByID(entityID);
		assert(entity != nullptr);

		EntityBase *parent = entity->getParent();
		if (parent != nullptr)
		{
			if (dynamic_cast<EntityGeometry *>(parent) != nullptr)
			{
				// this is a geometry entity as parent of the updated entity -> needs to be updated

				if (affectedParentMap.count(parent->getEntityID()) != 0)
				{
					// We only need to add each entity once
					affectedParentList.remove(parent->getEntityID());
				}

				affectedParentList.push_back(parent->getEntityID());
				affectedParentMap[parent->getEntityID()] = true;
			}
		}
	}

	// Now we send the update message to the Modeling Service
	std::list<ot::UID> entityIDs;
	std::list<ot::UID> entityVersions;
	std::list<ot::UID> entityInfoIDList;
	std::list<ot::UID> entityInfoVersionList;

	for (auto entityID : affectedParentList)
	{
		EntityGeometry *entity = dynamic_cast<EntityGeometry *>(getEntityByID(entityID));
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			entityIDs.push_back(entity->getEntityID());
			entityVersions.push_back(entity->getEntityStorageVersion());
		}

		determineIDandVersionForEntityWithChildren(entity, entityInfoIDList, entityInfoVersionList);
	}

	if (affectedParentList.empty())
	{
		// No further update is required. Refresh selection
		Application::instance()->getNotifier()->refreshSelection(visualizationModelID);
		return "";
	}

	ot::JsonDocument notify;
	notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ParentNeedsUpdate, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDs, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersions, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityInfoID, ot::JsonArray(entityInfoIDList, notify.GetAllocator()), notify.GetAllocator());
	notify.AddMember(OT_ACTION_PARAM_MODEL_EntityInfoVersion, ot::JsonArray(entityInfoVersionList, notify.GetAllocator()), notify.GetAllocator());

	return notify.toJson();
}

int Model::getServiceID()
{
	return Application::instance()->getServiceID();
}

int Model::getSessionCount()
{
	return Application::instance()->getSessionCount();
}

ModelState * Model::getStateManager()
{
	if (stateManager == nullptr) {
		stateManager = new ModelState(getSessionCount(), static_cast<unsigned int>(getServiceID()));
	}
	return stateManager;
}

void Model::setStateMangager(ModelState* state)
{
	entityMap.clear();

	stateManager = state;
	stateManager->openProject();

	auto doc = bsoncxx::builder::basic::document{};

	if (!GetDocumentFromEntityID(0, doc))
	{
		displayMessage("ERROR: Unable to read most recent model from storage: " + projectName + "\n");
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	ot::UID schemaVersion = (ot::UID)DataBase::getIntFromView(doc_view, "SchemaVersion_Model");
	assert(schemaVersion == 2);

	long long entityRootId = doc_view["entityRoot"].get_int64();
	setModelStorageVersion((ot::UID)DataBase::getIntFromView(doc_view, "Version"));
	
	std::list<ot::UID> prefetchIds;
	getStateManager()->getListOfTopologyEntites(prefetchIds);
	OT_LOG_D("Loading entities of model state.");
	prefetchDocumentsFromStorage(prefetchIds);
	OT_LOG_D("Building model state.");
	// Now read the root entity (this will implicitly also recursively load all other subentities as well
	if (entityRootId != -1)
	{
		entityRoot = dynamic_cast<EntityContainer*>(readEntityFromEntityID(nullptr, entityRootId, entityMap));
	}
}

void Model::determineIDandVersionForEntityWithChildren(EntityBase *entity, std::list<ot::UID> &entityInfoIDList, std::list<ot::UID> &entityInfoVersionList)
{	
	entityInfoIDList.push_back(entity->getEntityID());
	entityInfoVersionList.push_back(getStateManager()->getCurrentEntityVersion(entity->getEntityID()));

	EntityGeometry *geometryEntity = dynamic_cast<EntityGeometry *>(entity);

	if (geometryEntity != nullptr)
	{
		entityInfoIDList.push_back(geometryEntity->getBrepStorageObjectID());
		entityInfoVersionList.push_back(getStateManager()->getCurrentEntityVersion(geometryEntity->getBrepStorageObjectID()));

		entityInfoIDList.push_back(geometryEntity->getFacetsStorageObjectID());
		entityInfoVersionList.push_back(getStateManager()->getCurrentEntityVersion(geometryEntity->getFacetsStorageObjectID()));
	}

	EntityContainer *containerEntity = dynamic_cast<EntityContainer *>(entity);

	if (containerEntity != nullptr)
	{
		for (auto child : containerEntity->getChildrenList())
		{
			determineIDandVersionForEntityWithChildren(child, entityInfoIDList, entityInfoVersionList);
		}
	}
}
