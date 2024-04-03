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

#pragma warning(disable:4251)

namespace ot {

	class Property;
	class Painter2D;

	class OT_GUI_API_EXPORT PropertyGroup : public Serializable {
		OT_DECL_NOCOPY(PropertyGroup)
	public:
		//! @brief Create empty group
		PropertyGroup();

		//! @brief Create empty group with name and title
		//! @param _name Will be set as name and title
		PropertyGroup(const std::string& _name);

		//! @brief Create empty group with name and title
		//! @param _name Group name
		//! @param _title Group title
		PropertyGroup(const std::string& _name, const std::string& _title);
		virtual ~PropertyGroup();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		std::string& name(void) { return m_name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		std::string& title(void) { return m_title; };
		const std::string& title(void) const { return m_title; };

		//! @brief Set the properties.
		//! This group takes ownership of the properties.
		void setProperties(const std::list<Property*>& _properties);

		//! @brief Add the property.
		//! This group takes ownership of the property.
		void addProperty(Property* _property);
		const std::list<Property*>& properties(void) const { return m_properties; };

		//! @brief Set the child groups.
		//! This group takes ownership of the groups.
		void setChildGroups(const std::list<PropertyGroup*>& _groups);

		//! @brief Add the provided group as a child.
		//! This group takes ownership of the child.
		void addChildGroup(PropertyGroup* _group);

		void setBackgroundColor(Color::DefaultColor _color) { this->setBackgroundColor(Color(_color)); };
		void setBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setBackgroundColor(Color(_r, _g, _b, _a)); };
		void setBackgroundColor(float _r, float _g, float _b, float _a = 1.f) { this->setBackgroundColor(Color(_r, _g, _b, _a)); };
		void setBackgroundColor(const Color& _color);
		void setBackgroundPainter(Painter2D* _painter);
		Painter2D* backgroundPainter(void) const { return m_backgroundPainter; };

		void setAlternateBackgroundColor(Color::DefaultColor _color) { this->setAlternateBackgroundColor(Color(_color)); };
		void setAlternateBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setAlternateBackgroundColor(Color(_r, _g, _b, _a)); };
		void setAlternateBackgroundColor(float _r, float _g, float _b, float _a = 1.f) { this->setAlternateBackgroundColor(Color(_r, _g, _b, _a)); };
		void setAlternateBackgroundColor(const Color& _color);
		void setAlternateBackgroundPainter(Painter2D* _painter);
		Painter2D* alternateBackgroundPainter(void) const { return m_alternateBackgroundPainter; };

		void clear(bool _keepGroups = false);

	private:
		std::string m_name;
		std::string m_title;
		Painter2D* m_backgroundPainter;
		Painter2D* m_alternateBackgroundPainter;
		std::list<Property*> m_properties;
		std::list<PropertyGroup*> m_childGroups;
	};

}
