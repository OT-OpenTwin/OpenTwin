// @otlicense
// File: PropertyHelper.h
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
#include "OTCore/Color.h"
#include "OTModelEntities/EntityProperties.h"
#include "OTModelEntities/EntityPropertiesItems.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

class EntityBase;

class OT_MODELENTITIES_API_EXPORT PropertyHelper
{
	OT_DECL_STATICONLY(PropertyHelper)
public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	OT_DECL_NODISCARD static bool hasProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	//! @brief Returns a pointer to the writable property base with the provided name and group name.
	//! @param _base Pointer to the entity containing the property.
	//! @param _name Name of the property to access.
	//! @param _groupName Optional group name. If not provided, the first property with the provided name will be returned regardless of its group.
	//! @throws ot::Exception::ObjectNotFound If no property with the provided name and group name exists.
	OT_DECL_NODISCARD static EntityPropertiesBase* getPropertyBase(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	//! @brief Returns a pointer to the read only property base with the provided name and group name.
	//! @param _base Pointer to the entity containing the property.
	//! @param _name Name of the property to access.
	//! @param _groupName Optional group name. If not provided, the first property with the provided name will be returned regardless of its group.
	//! @throws ot::Exception::ObjectNotFound If no property with the provided name and group name exists.
	OT_DECL_NODISCARD static const EntityPropertiesBase* getPropertyBase(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	//! @brief Sets the visibility of the property with the provided name and group name.
	//! @param _visible Whether the property should be visible or not.
	//! @param _base Pointer to the entity containing the property.
	//! @param _name Name of the property to access.
	//! @param _groupName Optional group name. If not provided, the first property with the provided name will be set regardless of its group.
	//! @throws ot::Exception::ObjectNotFound If no property with the provided name and group name exists.
	static void setPropertyVisible(bool _visible, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	//! @brief Returns whether the property with the provided name and group name is visible.
	//! @param _base Pointer to the entity containing the property.
	//! @param _name Name of the property to access.
	//! @param _groupName Optional group name. If not provided, the first property with the provided name will be accessed regardless of its group.
	//! @throws ot::Exception::ObjectNotFound If no property with the provided name and group name exists.
	OT_DECL_NODISCARD static bool getPropertyVisible(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Value getter

	OT_DECL_NODISCARD static double getDoublePropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static std::string getStringPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static std::string getSelectionPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static bool getBoolPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static ot::Color getColourPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const ot::Painter2D* getPainterPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static int32_t getIntegerPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static std::string getProjectPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static std::string getEntityListPropertyValueName (const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static ot::UID getEntityListPropertyValueID (const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Value setter

	OT_DECL_NODISCARD static void setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setSelectionPropertyValue(const std::list<std::string>& _values, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setColourPropertyValue(const ot::Color& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setPainterPropertyValue(const ot::Painter2D* _painter, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setIntegerPropertyValue(int32_t _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static void setProjectPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Writable property access

	//! @brief Returns a pointer to the property with the provided name and group name.
	//! @tparam T The expected property type. Must be a subclass of EntityPropertiesBase.
	//! @param _base Pointer to the entity containing the property.
	//! @param _name Name of the property to access.
	//! @param _groupName Optional group name. If not provided, the first property with the provided name will be returned regardless of its group.
	//! @throws ot::Exception::ObjectNotFound If no property with the provided name and group name exists.
	//! @throws ot::Exception::InvalidType If the property with the provided name and group name is not of type T.
	template<typename T> OT_DECL_NODISCARD static T* getProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "")
	{
		T* actualProperty = dynamic_cast<T*>(getPropertyBase(_base, _name, _groupName));
		if (actualProperty == nullptr)
		{
			throw ot::Exception::InvalidType("Proprety cast failed { \"Name\": \"" + _name + "\", \"Group\": \"" + _groupName + "\"}");
		}
		return actualProperty;
	}

	OT_DECL_NODISCARD static EntityPropertiesDouble* getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesString* getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesSelection* getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesBoolean* getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesColor* getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesInteger* getIntegerProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesGuiPainter* getPainterProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesEntityList* getEntityListProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static EntityPropertiesProjectList* getEntityProjectListProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

	// ###########################################################################################################################################################################################################################################################################################################################

	// Read-only property access

	template<typename T> OT_DECL_NODISCARD static const T* getProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "")
	{
		const T* actualProperty = dynamic_cast<const T*>(getPropertyBase(_base, _name, _groupName));
		if (actualProperty == nullptr)
		{
			throw ot::Exception::InvalidType("Proprety cast failed { \"Name\": \"" + _name + "\", \"Group\": \"" + _groupName + "\"}");
		}
		return actualProperty;
	}
	
	OT_DECL_NODISCARD static const EntityPropertiesDouble* getDoubleProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesString* getStringProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesSelection* getSelectionProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesBoolean* getBoolProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesColor* getColourProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesInteger* getIntegerProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesGuiPainter* getPainterProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesEntityList* getEntityListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");
	OT_DECL_NODISCARD static const EntityPropertiesProjectList* getEntityProjectListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName = "");

};

