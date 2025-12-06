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
#include "DataBase.h"

static EntityFactoryRegistrar<EntityGraphicsScene> registrar(EntityGraphicsScene::className());

EntityGraphicsScene::EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityContainer(ID, parent, obs, ms), m_sceneFlags(SceneFlag::DefaultFlags)
{
	ot::VisualisationTypes visTypes;
	visTypes.addGraphicsViewVisualisation();
	setVisualizationTypes(visTypes, true);

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

void EntityGraphicsScene::setGraphicsPickerKey(const std::string& _key) {
	if (m_graphicsPickerKey != _key) {
		m_graphicsPickerKey = _key;
		setModified();
	}
}

void EntityGraphicsScene::setSceneFlags(const SceneFlags& _flags) {
	if (m_sceneFlags != _flags) {
		m_sceneFlags = _flags;
		setModified();
	}
}

void EntityGraphicsScene::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityContainer::addStorageData(_storage);
	_storage.append(bsoncxx::builder::basic::kvp("GraphicsPickerKey", m_graphicsPickerKey));
	if (m_sceneFlags != SceneFlag::DefaultFlags) {
		_storage.append(bsoncxx::builder::basic::kvp("SceneFlags", static_cast<int64_t>(m_sceneFlags.underlying())));
	}
}

void EntityGraphicsScene::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityContainer::readSpecificDataFromDataBase(_docView, _entityMap);
	auto keyIt = _docView.find("GraphicsPickerKey");
	if (keyIt != _docView.end()) {
		m_graphicsPickerKey = keyIt->get_utf8().value.data();
	}
	else {
		// Legacy support: Use owning service as picker key
		auto lst = getServicesForCallback(EntityBase::Callback::DataNotify);
		if (lst.empty()) {
			OT_LOG_W("Graphics scene entity has no GraphicsPickerKey and no owning service set { \"ID\": " + std::to_string(getEntityID()) + ", \"Name\": \"" + getName() + "\" }. Defaulting to Model");
			m_graphicsPickerKey = OT_INFO_SERVICE_TYPE_MODEL;
		}
		else {
			m_graphicsPickerKey = lst.front();
		}
	}

	auto flagsIt = _docView.find("SceneFlags");
	if (flagsIt != _docView.end()) {
		m_sceneFlags = SceneFlags(DataBase::getIntFromView(_docView, "SceneFlags"));
	}
}
