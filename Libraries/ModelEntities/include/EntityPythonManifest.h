// @otlicense
// File: EntityPythonManifest.h
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

// OpenTwin header
#include "IVisualisationText.h"
#include "EntityBase.h"

class __declspec(dllexport) EntityPythonManifest : public IVisualisationText, public EntityBase
{
	friend class FixturePythonManifest;
public:
	EntityPythonManifest() : EntityPythonManifest(0, nullptr, nullptr, nullptr) {};
	EntityPythonManifest(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
	virtual std::string getClassName(void) const override { return EntityPythonManifest::className(); };
	static std::string className() { return "EntityPythonManifest"; };

	// Inherited via EntityBase
	bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; }
	entityType getEntityType() const override { return TOPOLOGY; }
	virtual void addVisualizationNodes() override;
	
	virtual void setEntityID(ot::UID id) override;

	// Inherited via IVisualisationText
	std::string getText() override;
	
	//! @brief Replaces the manifest text. If the environment has changed a new manifest ID is generated.
	void setText(const std::string& _text) override;
	ot::TextEditorCfg createConfig(const ot::VisualisationCfg& _visualizationConfig) override;
	bool visualiseText() override { return true; }
	
	ot::UID getManifestID() const { return m_manifestID; }
	void setManifestID(ot::UID _id);
	
	//! @brief  Creates a new manifest ID and returns it
	ot::UID generateNewManifestID();
	std::optional <std::list<std::string>> getManifestPackages() { return getPackageList(m_manifestText); }
	//! @brief Replaces manifest text without generating a new manifest ID
	void replaceManifest(const std::string& _newManifestText);

private:
	ot::UID m_manifestID = ot::invalidUID;
	std::string m_manifestText;

	bool environmentHasChanged(const std::string& _newContent);
	std::string trim(const std::string& _line);
	std::optional <std::list<std::string>> getPackageList(const std::string _text);

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;
};
