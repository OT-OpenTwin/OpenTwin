// @otlicense
// File: EntityFileRawData.h
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
#include "EntityFile.h"

class OT_MODELENTITIES_API_EXPORT EntityFileRawData : public EntityFile {
public:
	EntityFileRawData() : EntityFileRawData(0, nullptr, nullptr, nullptr) {};
	EntityFileRawData(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
	virtual ~EntityFileRawData() = default;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	static std::string className() { return "EntityFileRawData"; };
	virtual std::string getClassName(void) const override { return EntityFileRawData::className(); };

	bool updateFromProperties() override;

protected:
	void setSpecializedProperties() override;

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	ot::ImageFileFormat m_format;

};