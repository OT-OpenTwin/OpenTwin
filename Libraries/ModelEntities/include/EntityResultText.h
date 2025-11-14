// @otlicense
// File: EntityResultText.h
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
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "OldTreeIcon.h"
#include "IVisualisationText.h"

class EntityResultTextData;

class __declspec(dllexport) EntityResultText : public EntityBase, public IVisualisationText
{
public:
	EntityResultText() : EntityResultText(0, nullptr, nullptr, nullptr) {};
	EntityResultText(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms);
	virtual ~EntityResultText();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(bool isHidden);
	
	virtual std::string getClassName(void) const override { return "EntityResultText"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	
	void createProperties(void);

	virtual bool updateFromProperties(void) override;

	void deleteTextData(void);

	void storeTextData(void);
	void releaseTextData(void);

	ot::UID getTextDataStorageId(void) { return m_textDataStorageId; }
	ot::UID getTextDataStorageVersion(void) { return m_textDataStorageVersion; }

	// Inherited via IVisualisationText
	void setText(const std::string &text) override;
	std::string getText() override;
	bool visualiseText() override;
	ot::TextEditorCfg createConfig(const ot::VisualisationCfg& _visualizationConfig) override;
	
private:
	void ensureTextDataLoaded(void);
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	EntityResultTextData *getTextData(void);

	EntityResultTextData *m_textData = nullptr;
	ot::UID m_textDataStorageId = 0;
	ot::UID m_textDataStorageVersion = 0;
};



