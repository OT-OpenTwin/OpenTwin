// @otlicense
// File: EntityProperties.h
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

#include <string>
#include <map>

#include "EntityPropertiesItems.h"

#include "OTGui/PropertyGridCfg.h"

class EntityBase;

namespace ot { class PropertyGroup; };

class OT_MODELENTITIES_API_EXPORT EntityProperties
{
public:
	EntityProperties() : m_needsUpdate(false) {};
	EntityProperties(const EntityProperties &other);
	virtual ~EntityProperties();

	void merge(EntityProperties& other);

	//! @brief Takes ownership of the property pointer.
	bool createProperty(EntityPropertiesBase *property, const std::string &group, bool addToFront = false);
	bool updateProperty(EntityPropertiesBase *property, const std::string &group);
	bool deleteProperty(const std::string &_name, const std::string& _groupName = "");
	bool propertyExists(const std::string &_name, const std::string& _groupName = "");

	EntityPropertiesBase* getProperty(const std::string& _name, const std::string& _groupName = "") { return getPropertyImpl(_name, _groupName); };
	const EntityPropertiesBase* getProperty(const std::string& _name, const std::string& _groupName = "") const { return getPropertyImpl(_name, _groupName); };

	void setNeedsUpdate(void) { m_needsUpdate = true; };
	bool anyPropertyNeedsUpdate(void) const { return m_needsUpdate; };
	void checkWhetherUpdateNecessary(void);
	void forceResetUpdateForAllProperties();

	void addToConfiguration(EntityBase *root, bool visibleOnly, ot::PropertyGridCfg& _config) const;
	void buildFromConfiguration(const ot::PropertyGridCfg& _config, EntityBase* root);
	void buildFromConfiguration(const ot::PropertyGroup* _groupConfig, EntityBase* root);

	std::string createJSON(EntityBase* root, bool visibleOnly) const;
	void buildFromJSON(const std::string& prop, EntityBase* root);

	void checkMatchingProperties(EntityProperties &other);
	void readFromProperties(const EntityProperties &other, EntityBase *root);

	EntityProperties& operator=(const EntityProperties &other);

	void setAllPropertiesReadOnly(void);
	void setAllPropertiesNonProtected(void);

	std::list<EntityPropertiesBase*> getListOfAllProperties(void);
	std::list<EntityPropertiesBase *> getListOfPropertiesWhichNeedUpdate(void);
	std::list<EntityPropertiesDouble *> getListOfNumericalProperties(void);
	size_t getNumberOfProperties(void) const { return m_properties.size(); }

	std::list<std::string> getListOfPropertiesForGroup(const std::string &group) const;

	static std::string createKey(const std::string& _name, const std::string& _group);
	bool isKey(const std::string _accesser) const;
	std::string extractNameFromKey(const std::string& _key) const;

private:
	void deleteAllProperties(void);
	EntityPropertiesBase* getPropertyImpl(const std::string& _name, const std::string& _groupName = "") const;

	bool m_needsUpdate;
	std::map<std::string, EntityPropertiesBase *> m_properties;
	std::list<EntityPropertiesBase *> m_propertiesList; // The storage in the list is redundant, but allows to keep the order of the properties
};
