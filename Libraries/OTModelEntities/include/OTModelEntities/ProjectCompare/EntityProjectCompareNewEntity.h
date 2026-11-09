// @otlicense
// File: EntityProjectCompareNewEntity.h
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
#include "OTModelEntities/EntityBase.h"

namespace ot
{

	class OT_MODELENTITIES_API_EXPORT EntityProjectCompareNewEntity : public EntityBase
	{
	public:
		EntityProjectCompareNewEntity() : EntityProjectCompareNewEntity(ot::invalidUID, nullptr, nullptr, nullptr) {};
		EntityProjectCompareNewEntity(ot::UID _id, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms);
		virtual ~EntityProjectCompareNewEntity();

		static std::string className() { return "EntityProjectCompareNewEntity"; };
		virtual std::string getClassName() const override { return EntityProjectCompareNewEntity::className(); };

		virtual entityType getEntityType() const override { return TOPOLOGY; };

		void createProperties();

		virtual bool updateFromProperties() override;

		virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Property setter / getter

		void setCompareEntityType(const std::string& _entityType);
		std::string getCompareEntityType() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected base class methods

	protected:
		virtual int getSchemaVersion() override { return 1; };

		virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	};

}