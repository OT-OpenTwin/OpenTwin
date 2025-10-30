// @otlicense
// File: EntityMeshTetInfo.h
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

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntityMeshTetInfo : public EntityBase
{
public:
	EntityMeshTetInfo() : EntityMeshTetInfo(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshTetInfo(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshTetInfo();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual std::string getClassName(void) const override { return "EntityMeshTetInfo"; };

	virtual entityType getEntityType(void) const override { return DATA; };

	void addGeneralData(ot::UID dataID, ot::UID dataVersion) { generalData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };
	void addFacesData(ot::UID dataID, ot::UID dataVersion) { facesData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };
	void addVolumeData(ot::UID dataID, ot::UID dataVersion) { volumeData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::list<std::pair<ot::UID, ot::UID>> generalData;
	std::list<std::pair<ot::UID, ot::UID>> facesData;
	std::list<std::pair<ot::UID, ot::UID>> volumeData;
};



