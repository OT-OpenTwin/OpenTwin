// @otlicense
// File: EntityMaterial.h
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

class __declspec(dllexport) EntityMaterial : public EntityBase
{
public:
	EntityMaterial() : EntityMaterial(0, nullptr, nullptr, nullptr) {};
	EntityMaterial(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms);
	virtual ~EntityMaterial();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	static std::string className() { return "EntityMaterial"; };
	virtual std::string getClassName(void) const override { return EntityMaterial::className(); };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};



