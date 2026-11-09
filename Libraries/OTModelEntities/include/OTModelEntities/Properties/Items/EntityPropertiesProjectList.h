// @otlicense
// File: EntityPropertiesProjectList.h
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
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class OT_MODELENTITIES_API_EXPORT EntityPropertiesProjectList : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "projectlist"; };

	EntityPropertiesProjectList() {};
	EntityPropertiesProjectList(const std::string& _name) { setName(_name); }
	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesProjectList(*this); };
	virtual eType getType() const override { return PROJECTLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesProjectList::typeString(); };

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	virtual bool hasSameValue(EntityPropertiesBase* other) const override { return true; };

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	static EntityPropertiesProjectList* createProperty(const std::string& _group, const std::string& _name, const std::string& _defaultValue, const std::string& defaultCategory, EntityProperties& properties);

	void setValue(const std::string& _value) { if (m_value != _value) setNeedsUpdate(); m_value = _value; }
	std::string getValue() const { return m_value; }
	static std::string getCurrentProjectPlaceholder() { return "<Current project>"; }
private:
	std::string m_value;
};
