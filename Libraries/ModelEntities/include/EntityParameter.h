// @otlicense
// File: EntityParameter.h
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
#include <vector>
#include <array>

class __declspec(dllexport) EntityParameter : public EntityBase
{
public:
	EntityParameter() : EntityParameter(0, nullptr, nullptr, nullptr, "") {};
	EntityParameter(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, const std::string &owner);
	virtual ~EntityParameter();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setValue(const std::string &value);
	std::string getValue(void);
	
	double getNumericValue(void);

	void createProperties(void);

	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) const override { return "EntityParameter"; };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool updateFromProperties(void) override;

	void addDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void addDependencyByIndex(ot::UID entityID, const std::string& propertyIndex);
	void removeDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void removeDependency(ot::UID entityID);
	bool hasDependency(ot::UID entityID);
	void removeAllDependencies(void);

	std::map<ot::UID, std::map<std::string, bool>> getDependencies(void) { return dependencyMap; }

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::map<ot::UID, std::map<std::string, bool>> dependencyMap;
};



