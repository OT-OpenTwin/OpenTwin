//! @file PropertyGroup.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

namespace ot {

	class Property;

	class OT_GUI_API_EXPORT PropertyGroup : public Serializable {
		OT_DECL_NOCOPY(PropertyGroup)
	public:
		PropertyGroup() {};
		PropertyGroup(const std::string& _name) : m_name(_name) {};
		PropertyGroup(const std::string& _name, const std::string& _title) : m_name(_name), m_title(_title) {};
		virtual ~PropertyGroup();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setProperties(const std::list<Property*>& _properties);
		void addProperty(Property* _property);
		const std::list<Property*>& properties(void) const { return m_properties; };

		void setName(const std::string& _name) { m_name = _name; };
		std::string& name(void) { return m_name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		std::string& title(void) { return m_title; };
		const std::string& title(void) const { return m_title; };

	private:
		std::string m_name;
		std::string m_title;
		std::list<Property*> m_properties;
	};

}
