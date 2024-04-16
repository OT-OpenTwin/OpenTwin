//! @file Property.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT PropertyBase : public Serializable {
	public:
		enum PropertyFlag {
			NoFlags = 0x0000, //! @brief No property flags set
			IsReadOnly = 0x0001, //! @brief Property is read only
			IsProtected = 0x0002, //! @brief Property is protected
			IsHidden = 0x0004, //! @brief Property is hidden to the user
			HasMultipleValues = 0x0008, //! @brief Property has multiple values
			HasInputError = 0x0010, //! @brief The value is invalid
			IsDeletable = 0x0020, //! @brief Property is deletable
			AllowCustomValues = 0x1000, //! @brief User may set user values (e.g. in the StringListProperty)
			AllowMultiselection = 0x2000  //! @brief User may select multiple values (e.g. in the StringListProperty)
		};
		typedef Flags<PropertyFlag> PropertyFlags;

		static std::string toString(PropertyFlag _flag);
		static PropertyFlag stringToFlag(const std::string& _flag);
		static std::list<std::string> toStringList(PropertyFlags _flags);
		static PropertyFlags stringListToFlags(const std::list<std::string>& _flags);

		PropertyBase(PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyBase(const std::string& _name, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyBase(const PropertyBase& _other);
		virtual ~PropertyBase() {};

		PropertyBase& operator = (const PropertyBase& _other);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPropertyName(const std::string& _name) { m_name = _name; };
		const std::string& propertyName(void) const { return m_name; };

		//! @brief Set the property title
		//! If no title set the name will be used as title
		void setPropertyTitle(const std::string& _title) { m_title = _title; };

		//! @brief Property title
		//! If no title set the name will be used as title
		const std::string& propertyTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		void setPropertyTip(const std::string& _tip) { m_tip = _tip; };
		std::string& propertyTip(void) { return m_tip; };
		const std::string& propertyTip(void) const { return m_tip; };

		void setPropertyFlag(PropertyFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setPropertyFlags(PropertyFlags _flags) { m_flags = _flags; };
		PropertyFlags& propertyFlags(void) { return m_flags; };
		const PropertyFlags& propertyFlags(void) const { return m_flags; };

		void setSpecialType(const std::string& _type) { m_specialType = _type; };
		const std::string& specialType(void) const { return m_specialType; };

		void setAdditionalPropertyData(const std::string& _data) { m_data = _data; };
		const std::string& additionalPropertyData(void) const { return m_data; };

	private:
		std::string m_tip;
		std::string m_name;
		std::string m_title;
		std::string m_specialType;
		std::string m_data;
		PropertyFlags m_flags;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::PropertyBase::PropertyFlag)