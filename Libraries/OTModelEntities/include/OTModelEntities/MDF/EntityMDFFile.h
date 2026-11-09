// @otlicense
// File: EntityMDFFile.h
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
#include "OTModelEntities/EntityContainer.h"

namespace ot
{

	class OT_MODELENTITIES_API_EXPORT EntityMDFFile : public EntityContainer
	{
		OT_DECL_NOCOPY(EntityMDFFile)
		OT_DECL_NOMOVE(EntityMDFFile)
	public:
		EntityMDFFile() : EntityMDFFile(0, nullptr, nullptr, nullptr) {};
		explicit EntityMDFFile(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
		virtual ~EntityMDFFile() = default;

		virtual entityType getEntityType() const override { return TOPOLOGY; };
		static std::string className() { return "EntityMDFFile"; };
		virtual std::string getClassName() const override { return EntityMDFFile::className(); };
		virtual bool getEntityBox(double&, double&, double&, double&, double&, double&) override { return false; };

		virtual bool updateFromProperties() override;

		virtual std::string serialiseAsJSON() override;
		virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept;

	protected:
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;
	};

}