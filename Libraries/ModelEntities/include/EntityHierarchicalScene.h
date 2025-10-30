// @otlicense
// File: EntityHierarchicalScene.h
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

class OT_MODELENTITIES_API_EXPORT EntityHierarchicalScene : public EntityContainer, public IVisualisationGraphicsView {
public:
	EntityHierarchicalScene() : EntityHierarchicalScene(0, nullptr, nullptr, nullptr, "") {};
	EntityHierarchicalScene(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string defaultName() { return "Project"; };
	static std::string className() { return "EntityHierarchicalScene"; };
	virtual std::string getClassName(void) const override { return EntityHierarchicalScene::className(); };

	virtual void addVisualizationNodes() override;

	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override { return true; };
};
