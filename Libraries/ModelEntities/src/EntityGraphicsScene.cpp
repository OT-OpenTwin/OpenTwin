// @otlicense
// File: EntityGraphicsScene.cpp
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

#include "EntityGraphicsScene.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"

static EntityFactoryRegistrar<EntityGraphicsScene> registrar(EntityGraphicsScene::className());

EntityGraphicsScene::EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityContainer(ID,parent,obs,ms,owner)
{
}

ot::GraphicsNewEditorPackage* EntityGraphicsScene::getGraphicsEditorPackage()
{
	ot::GraphicsNewEditorPackage* editor = new ot::GraphicsNewEditorPackage(getName(), getName());
	editor->setPickerKey(m_graphicsPickerKey);
	return editor;
}

bool EntityGraphicsScene::visualiseGraphicsView()
{
	return true;
}

void EntityGraphicsScene::addVisualizationNodes()
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ContainerVisible";
	treeIcons.hiddenIcon = "ContainerHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::BasicServiceInformation ownerInfo(this->getOwningService());
	ownerInfo.addToJsonObject(doc, doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addGraphicsViewVisualisation();
	visTypes.addToJsonObject(doc, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

}

void EntityGraphicsScene::setGraphicsPickerKey(const std::string& _key) {
	if (m_graphicsPickerKey != _key) {
		m_graphicsPickerKey = _key;
		setModified();
	}
}

void EntityGraphicsScene::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityContainer::addStorageData(_storage);
	_storage.append(bsoncxx::builder::basic::kvp("GraphicsPickerKey", m_graphicsPickerKey));
}

void EntityGraphicsScene::readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityContainer::readSpecificDataFromDataBase(_docView, _entityMap);
	auto keyIt = _docView.find("GraphicsPickerKey");
	if (keyIt != _docView.end()) {
		m_graphicsPickerKey = keyIt->get_utf8().value.data();
	}
	else {
		// Legacy support: Use owning service as picker key
		m_graphicsPickerKey = getOwningService();
		if (m_graphicsPickerKey.empty()) {
			OT_LOG_W("Graphics scene entity has no GraphicsPickerKey and no owning service set { \"ID\": " + std::to_string(getEntityID()) + ", \"Name\": \"" + getName() + "\" }");
		}
	}
}
