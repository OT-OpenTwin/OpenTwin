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

// OpenTwin header
#include "OTGui/Properties/PropertyGridCfg.h"
#include "OTModelEntities/EntityPropertiesItems.h"

// std header
#include <map>
#include <string>

#pragma warning(disable : 4251)

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
	bool createProperty(EntityPropertiesBase *property, const std::string &group, bool addToFront = false, std::string insertBeforeGroup = "");
	bool updateProperty(EntityPropertiesBase *property, const std::string &group);
	bool deleteProperty(const std::string &_name, const std::string& _groupName = "");
	bool propertyExists(const std::string &_name, const std::string& _groupName = "");

	//! @brief Returns a writable pointer to the property with the provided name and group.
	//! If no group is provided, the first property with the provided name will be returned.
	//! The caller needs to check whether the returned pointer is valid.
	//! The ownership of the property pointer remains with the EntityProperties object.
	//! @param _name Property name.
	//! @param _groupName Group name (optional).
	OT_DECL_NODISCARD EntityPropertiesBase* getProperty(const std::string& _name, const std::string& _groupName = "") { return getPropertyImpl(_name, _groupName); };

	//! @brief Returns a read only pointer to the property with the provided name and group.
	//! If no group is provided, the first property with the provided name will be returned.
	//! The caller needs to check whether the returned pointer is valid.
	//! The ownership of the property pointer remains with the EntityProperties object.
	//! @param _name Property name.
	//! @param _groupName Group name (optional).
	OT_DECL_NODISCARD const EntityPropertiesBase* getProperty(const std::string& _name, const std::string& _groupName = "") const { return getPropertyImpl(_name, _groupName); };

	void setNeedsUpdate() { m_needsUpdate = true; };
	bool anyPropertyNeedsUpdate() const { return m_needsUpdate; };
	void checkWhetherUpdateNecessary();
	void forceResetUpdateForAllProperties();

	void addToConfiguration(EntityBase *root, bool visibleOnly, ot::PropertyGridCfg& _config) const;
	void buildFromConfiguration(const ot::PropertyGridCfg& _config, EntityBase* root);
	void buildFromConfiguration(const ot::PropertyGroup* _groupConfig, EntityBase* root);

	std::string createJSON(EntityBase* root, bool visibleOnly) const;
	void buildFromJSON(const std::string& prop, EntityBase* root);

	void checkMatchingProperties(EntityProperties &other);
	void readFromProperties(const EntityProperties &other, EntityBase *root);

	EntityProperties& operator=(const EntityProperties &other);

	void setAllPropertiesReadOnly();
	void setAllPropertiesNonProtected();

	std::list<EntityPropertiesBase*> getListOfAllProperties();
	std::list<EntityPropertiesBase *> getListOfPropertiesWhichNeedUpdate();
	std::list<EntityPropertiesDouble *> getListOfNumericalProperties();
	size_t getNumberOfProperties() const { return m_properties.size(); }

	std::list<std::string> getListOfPropertiesForGroup(const std::string &group) const;

	static std::string createKey(const std::string& _name, const std::string& _group);
	bool isKey(const std::string _accesser) const;
	std::string extractNameFromKey(const std::string& _key) const;

private:
	void deleteAllProperties();
	EntityPropertiesBase* getPropertyImpl(const std::string& _name, const std::string& _groupName = "") const;

	bool m_needsUpdate;
	std::map<std::string, EntityPropertiesBase *> m_properties;
	std::list<EntityPropertiesBase *> m_propertiesList; // The storage in the list is redundant, but allows to keep the order of the properties
};
