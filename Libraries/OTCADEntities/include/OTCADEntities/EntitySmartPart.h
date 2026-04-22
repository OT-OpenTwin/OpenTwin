// @otlicense
// File: EntitySmartPart.h
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

#include "OTModelEntities/EntityBinaryData.h"

#include "OTCADEntities/EntityGeometry.h"

class OT_CADENTITIES_API_EXPORT EntitySmartPart : public EntityGeometry
{
public:
	EntitySmartPart() : EntitySmartPart(0, nullptr, nullptr, nullptr) {};
	EntitySmartPart(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntitySmartPart();

	virtual void removeChild(EntityBase* child) override;

	static std::string className() { return "EntitySmartPart"; };
	virtual std::string getClassName(void) const override { return EntitySmartPart::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual void storeToDataBase(void) override;

	EntityBinaryData* getData(void);

	virtual void updatePart(void) {};

	virtual void createProperties(int colorR, int colorG, int colorB, const std::string& geometryFolder, ot::UID geometryFolderID, const std::string& materialsFolder, ot::UID materialsFolderID) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
	void ensureDataLoaded(void);
	void storeData(void);
	void addPartProperty(const std::string& name, const std::string& value);
	virtual void createPartProperties(void) {};

	EntityBinaryData* m_data = nullptr;
	ot::UID m_dataStorageId = 0;
	ot::UID m_dataStorageVersion = 0;
};


