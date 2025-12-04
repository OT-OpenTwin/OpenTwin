// @otlicense
// File: EntityFileText.h
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
#include "EntityFile.h"
#include "OTCore/TextEncoding.h"
#include "OTGui/TextEditorCfg.h"
#include "IVisualisationText.h"

class __declspec(dllexport) EntityFileText : public EntityFile, public IVisualisationText
{
public:
	EntityFileText() : EntityFileText(0, nullptr, nullptr, nullptr) {};
	EntityFileText(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
	virtual ~EntityFileText() = default;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	static std::string className() { return "EntityFileText"; };
	virtual std::string getClassName(void) const override { return EntityFileText::className(); };

	void setTextEncoding(ot::TextEncoding::EncodingStandard _encoding);
	ot::TextEncoding::EncodingStandard getTextEncoding();

	// Text visualisation interface

	std::string getText(void) override;

	//! @brief Setting a text will change the underlying data entity. If the modelstate is set, the data entity is directly stored and added to the modelstate.
	void setText(const std::string& _text) override;
	bool visualiseText() override { return true; }
	ot::TextEditorCfg createConfig(const ot::VisualisationCfg& _visualizationConfig) override;

	bool updateFromProperties() override;

protected:
	void setSpecializedProperties() override;
	
	//! @brief Must be called after updateFromProperties
	bool requiresDataUpdate();
	
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	void readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	bool m_requiresDataUpdate = false;
	ot::TextEncoding::EncodingStandard m_encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
};
