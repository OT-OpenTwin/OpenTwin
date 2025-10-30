// @otlicense
// File: MaterialHandler.h
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
#include "SelectionChangedObserver.h"
#include "OTGui/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "EntityMaterial.h"

class MaterialHandler : public SelectionChangedObserver
{
public:
	MaterialHandler();

	void addButtons(ot::components::UiComponent* _uiComponent);
	static EntityMaterial* createNewMaterial(const std::string& _materialName);
	
private:
	const std::string c_groupMaterial = "Material";
	ot::ToolBarButtonCfg m_buttonCreateMaterial;
	ot::ToolBarButtonCfg m_buttonShowByMaterial;
	ot::ToolBarButtonCfg m_buttonMaterialMissing;

	ot::ButtonHandler m_buttonHandler;
	void handleShowByMaterial();
	void handleShowMaterialMissing();	
	void handleCreateNewMaterial();

	// Inherited via SelectionChangedObserver
	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;

};
