// @otlicense
// File: EntityGraphicsScene.h
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

#pragma once
#include "EntityContainer.h"
#include "IVisualisationGraphicsView.h"

class __declspec(dllexport) EntityGraphicsScene : public EntityContainer, public IVisualisationGraphicsView
{
public:
	enum SceneFlag : int64_t {
		NoFlags                       = 0 << 0, //! @brief No special flags.
		AllowConnectionsOnConnections = 1 << 0, //! @brief Allows connections on connections.

		DefaultFlags = NoFlags //! @brief Default flags.
	};
	typedef ot::Flags<SceneFlag, int64_t> SceneFlags;

	EntityGraphicsScene() : EntityGraphicsScene(0, nullptr, nullptr, nullptr) {};
	EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	static std::string className() { return "EntityGraphicsScene"; };
	virtual std::string getClassName(void) const override { return EntityGraphicsScene::className(); };
	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override;
	void addVisualizationNodes() override;

	virtual void setGraphicsPickerKey(const std::string& _key) override;
	virtual std::string getGraphicsPickerKey() const override { return m_graphicsPickerKey; };

	void setSceneFlags(const SceneFlags& _flags);
	const SceneFlags& getSceneFlags() const { return m_sceneFlags; };

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	std::string m_graphicsPickerKey;
	SceneFlags m_sceneFlags;
};

OT_ADD_FLAG_FUNCTIONS(EntityGraphicsScene::SceneFlag, EntityGraphicsScene::SceneFlags)