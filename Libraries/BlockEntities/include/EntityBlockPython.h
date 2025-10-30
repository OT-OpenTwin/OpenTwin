// @otlicense
// File: EntityBlockPython.h
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

#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockPython : public EntityBlock
{
public:
	EntityBlockPython() : EntityBlockPython(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockPython"; }
	virtual std::string getClassName(void) const override { return EntityBlockPython::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	void createProperties(const std::string& scriptFolder, ot::UID scriptFolderID);
	std::string getSelectedScript();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	static const std::string getIconName() { return "Script.svg"; }
private:
	const std::string _propertyNameScripts = "Scripts";

	void updateBlockAccordingToScriptHeader();
	void resetBlockRelatedAttributes();
};
