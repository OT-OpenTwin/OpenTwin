// @otlicense

#include "stdafx.h"
#include "MaterialHandler.h"
#include "EntityGeometry.h"
#include "Model.h"
#include "Application.h"
#include "MicroserviceNotifier.h"
#include "QueuingHttpRequestsRAII.h"
#include "GeometryOperations.h"

MaterialHandler::MaterialHandler() {
	const std::string pageName = Application::getToolBarPageName();
	m_buttonCreateMaterial = ot::ToolBarButtonCfg(pageName, c_groupMaterial, "Create Material", "Default/AddMaterial");
	m_buttonCreateMaterial.setButtonLockFlags(ot::LockType::ModelWrite);
	
	m_buttonShowByMaterial = ot::ToolBarButtonCfg(pageName, c_groupMaterial, "Show By Material", "Default/ShowByMaterial");
	m_buttonShowByMaterial.setButtonLockFlags(ot::LockType::ModelRead);

	m_buttonMaterialMissing = ot::ToolBarButtonCfg(pageName, c_groupMaterial, "Material Missing", "Default/ShowMaterialMissing");
	m_buttonMaterialMissing.setButtonLockFlags(ot::LockType::ModelRead);

	m_buttonHandler.connectToolBarButton(m_buttonCreateMaterial, this, &MaterialHandler::handleCreateNewMaterial);
	m_buttonHandler.connectToolBarButton(m_buttonShowByMaterial, this, &MaterialHandler::handleShowByMaterial);
	m_buttonHandler.connectToolBarButton(m_buttonMaterialMissing, this, &MaterialHandler::handleShowMaterialMissing);
}

void MaterialHandler::addButtons(ot::components::UiComponent* _uiComponent)
{
	_uiComponent->addMenuGroup(Application::getToolBarPageName(), c_groupMaterial);

	_uiComponent->addMenuButton(m_buttonCreateMaterial);
	_uiComponent->addMenuButton(m_buttonShowByMaterial);
	_uiComponent->addMenuButton(m_buttonMaterialMissing);
}

EntityMaterial* MaterialHandler::createNewMaterial(const std::string& _materialName)
{
	Model* model = Application::instance()->getModel();

	EntityContainer* entityMaterialRoot = dynamic_cast<EntityContainer*>(model->findEntityFromName(model->getMaterialRootName()));
	if (entityMaterialRoot == nullptr)
	{
		entityMaterialRoot = new EntityContainer(model->createEntityUID(), nullptr, model, model->getStateManager(), Application::instance()->getServiceName());
		entityMaterialRoot->setName(model->getMaterialRootName());

		GeometryOperations::EntityList allNewEntities;
		model->addEntityToModel(entityMaterialRoot->getName(), entityMaterialRoot, model->getRootNode(), true, allNewEntities);

		model->addVisualizationContainerNode(entityMaterialRoot->getName(), entityMaterialRoot->getEntityID(), entityMaterialRoot->getEditable());
	}

	EntityMaterial* materialItem = new EntityMaterial(model->createEntityUID(), entityMaterialRoot, model, model->getStateManager(), Application::instance()->getServiceName());

	materialItem->setName(_materialName);
	materialItem->setEditable(true);
	entityMaterialRoot->addChild(materialItem);

	model->addEntityToMap(materialItem);

	// Now we create properties for the mesh
	materialItem->createProperties();

	return materialItem;
}

void MaterialHandler::handleShowByMaterial() {
	std::map<std::string, ot::UID> materialNameToIDMap;
	std::map<ot::UID, std::string> materialIdToNameMap;

	Model* model = Application::instance()->getModel();

	for (auto selEntityID : Application::instance()->getSelectionHandler().getSelectedEntityIDs()) {
		EntityMaterial* entityMaterial = dynamic_cast<EntityMaterial*>(model->getEntityByID(selEntityID));

		if (entityMaterial != nullptr) {
			materialNameToIDMap[entityMaterial->getName()] = entityMaterial->getEntityID();
			materialIdToNameMap[entityMaterial->getEntityID()] = entityMaterial->getName();
		}
	}

	// Loop through all geometry objects and put them in the visible / unvisible lists according to their material
	std::list<ot::UID> visible, hidden;
	auto& entityMap = model->getAllEntitiesByUID();
	for (auto entity : entityMap) {
		EntityGeometry* geometryEntity = dynamic_cast<EntityGeometry*>(entity.second);

		// Now we filter out all geometry entities which have a geometry entity as parent (e.g. children of boolean operations)
		if (geometryEntity != nullptr) {
			if (geometryEntity->getParent() != nullptr) {
				EntityGeometry* geometryParent = dynamic_cast<EntityGeometry*>(geometryEntity->getParent());
				if (geometryParent != nullptr) {
					hidden.push_back(geometryEntity->getEntityID());
					geometryEntity = nullptr; // We discard this child entity
				}
			}
		}

		if (geometryEntity != nullptr) {
			EntityPropertiesEntityList* material = dynamic_cast<EntityPropertiesEntityList*>(geometryEntity->getProperties().getProperty("Material"));

			if (material != nullptr) {
				// This entity has a material property assigned
				if (materialNameToIDMap.empty()) {
					// This entity will be visible, if the material is still undefined
					if (material->getValueName().empty()) {
						visible.push_back(geometryEntity->getEntityID());
					}
					else {
						hidden.push_back(geometryEntity->getEntityID());
					}
				}
				else {
					// This entity will be visible, if its material matches any one of the selected materials
					ot::UID materialID = 0;

					if (materialIdToNameMap.count(material->getValueID()) != 0) {
						materialID = material->getValueID();
					}
					else {
						if (materialNameToIDMap.count(material->getValueName()) != 0) {
							materialID = materialNameToIDMap[material->getValueName()];
						}
					}

					if (materialID != 0) {
						visible.push_back(geometryEntity->getEntityID());
					}
					else {
						hidden.push_back(geometryEntity->getEntityID());
					}
				}
			}
			else {
				hidden.push_back(geometryEntity->getEntityID());
			}
		}
	}

	// Now we need to send the list of the visible entities (all others will be hidden) to the UI
	model->setShapeVisibility(visible, hidden);
}

void MaterialHandler::handleShowMaterialMissing() {
	std::map<std::string, ot::UID> materialNameToIDMap;
	std::map<ot::UID, std::string> materialIdToNameMap;

	Model* model = Application::instance()->getModel();
	auto& entityMap = model->getAllEntitiesByUID();
	for (auto entity : entityMap) {
		EntityMaterial* entityMaterial = dynamic_cast<EntityMaterial*>(model->getEntityByID(entity.first));

		if (entityMaterial != nullptr) {
			materialNameToIDMap[entity.second->getName()] = entity.second->getEntityID();
			materialIdToNameMap[entity.second->getEntityID()] = entity.second->getName();
		}
	}

	// Loop through all geometry objects and put them in the visible / unvisible lists according to their material
	std::list<ot::UID> visible, hidden;

	for (auto entity : entityMap) {
		EntityGeometry* geometryEntity = dynamic_cast<EntityGeometry*>(entity.second);

		// Now we filter out all geometry entities which have a geometry entity as parent (e.g. children of boolean operations)
		if (geometryEntity != nullptr) {
			if (geometryEntity->getParent() != nullptr) {
				EntityGeometry* geometryParent = dynamic_cast<EntityGeometry*>(geometryEntity->getParent());
				if (geometryParent != nullptr) {
					hidden.push_back(geometryEntity->getEntityID());
					geometryEntity = nullptr; // We discard this child entity
				}
			}
		}

		if (geometryEntity != nullptr) {
			EntityPropertiesEntityList* material = dynamic_cast<EntityPropertiesEntityList*>(geometryEntity->getProperties().getProperty("Material"));

			if (material != nullptr) {
				bool materialExists = false;

				if (materialIdToNameMap.count(material->getValueID()) != 0) {
					materialExists = true;
				}
				else {
					if (materialNameToIDMap.count(material->getValueName()) != 0) {
						materialExists = true;
					}
				}

				if (materialExists) {
					hidden.push_back(geometryEntity->getEntityID());
				}
				else {
					visible.push_back(geometryEntity->getEntityID());
				}
			}
		}
		else {
			if (entity.second != nullptr) {
				hidden.push_back(entity.second->getEntityID());
			}
		}
	}

	// Now we need to send the list of the visible entities (all others will be hidden) to the UI	
	model->setShapeVisibility(visible, hidden);
}

void MaterialHandler::handleCreateNewMaterial() {
	QueuingHttpRequestsRAII enableQueuingHttpRequests;

	Model* model = Application::instance()->getModel();

	std::map<std::string, bool> allEntities = model->getListOfEntityNames();
	int count = 1;

	std::string materialName;

	do {
		materialName = model->getMaterialRootName() + "/material" + std::to_string(count);
		count++;

	} while (allEntities.find(materialName) != allEntities.end());

	EntityMaterial* materialItem = createNewMaterial(materialName);

	// Here we also need to add a new visualitation container item to the visualization model
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "MaterialVisible";
	treeIcons.hiddenIcon = "MaterialHidden";
	ot::UID visualizationModelID = model->getVisualizationModel();
	Application::instance()->getNotifier()->addVisualizationContainerNode(visualizationModelID, materialName, materialItem->getEntityID(), treeIcons, materialItem->getEditable());

	model->setModified();

	model->updatePropertyGrid(); // We need to update the property grid to make sure that the new material will become visible in the material selection

	model->modelChangeOperationCompleted("create new material");
}

void MaterialHandler::updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons)
{
	bool seriesSelected = false;
	for (EntityBase* selectedEntity : _selectedEntities)
	{
		EntityMaterial* materialEntity = dynamic_cast<EntityMaterial*>(selectedEntity);
		if (materialEntity != nullptr)
		{
			seriesSelected = true;
			break;
		}
	}

	if (seriesSelected)
	{
		_enabledButtons.push_back(m_buttonShowByMaterial.getFullPath());
	}
	else
	{
		_disabledButtons.push_back(m_buttonShowByMaterial.getFullPath());
	}
}

