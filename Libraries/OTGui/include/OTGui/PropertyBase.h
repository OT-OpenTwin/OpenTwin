// @otlicense

//! @file Property.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"

// std header
#include <map>
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class PropertyBase
	//! @brief The PropertyBase class is used to hold general Property information.
	class OT_GUI_API_EXPORT PropertyBase : public Serializable {
		OT_DECL_DEFCOPY(PropertyBase)
		OT_DECL_DEFMOVE(PropertyBase)
	public:
		//! @enum PropertyFlag
		//! @brief The PropertyFlag enum contains different settings for properties.
		enum PropertyFlag {
			NoFlags             = 0 << 0, //! @brief No property flags set
			IsReadOnly          = 1 << 0, //! @brief Property is read only
			IsProtected         = 1 << 1, //! @brief Property is protected
			IsHidden            = 1 << 2, //! @brief Property is hidden to the user
			HasMultipleValues   = 1 << 3, //! @brief Property has multiple values
			HasInputError       = 1 << 4, //! @brief The value is invalid
			IsDeletable         = 1 << 5, //! @brief Property is deletable
			AllowCustomValues   = 1 << 6, //! @brief User may set user values (e.g. in the StringListProperty)
			AllowMultiselection = 1 << 7, //! @brief User may select multiple values (e.g. in the StringListProperty)
			NoSerialize         = 1 << 8  //! @brief Property will not be concidered when serializing parent object.
		};

		//! @typedef PropertyFlags
		//! @copybrief PropertyBase::PropertyFlag
		typedef Flags<PropertyFlag> PropertyFlags;

		//! @brief Merge mode used when merging with other property manager.
		enum MergeFlag {
			NoMerge = 0 << 0, //! @brief No merge flags.
			MergeValues = 1 << 0, //! @brief Merge values.
			MergeConfig = 1 << 1, //! @brief Merge configuration (e.g. Flags, Name, ...).
			AddMissing = 1 << 2, //! @brief Add missing entries.

			//! @brief All merge flags set.
			FullMerge = MergeValues | MergeConfig | AddMissing
		};
		typedef Flags<MergeFlag> MergeMode;

		//! @brief Creates a string representation of the provided PropertyFlag.
		static std::string toString(PropertyFlag _flag);

		//! @brief Returns the PropertyFlag that is represented by the string.
		static PropertyFlag stringToFlag(const std::string& _flag);

		//! @brief Creates a list containing strings that represent the set PropertyFlags.
		static std::list<std::string> toStringList(PropertyFlags _flags);

		//! @brief Returns the PropertyFlags that are represented by the string list.
		static PropertyFlags stringListToFlags(const std::list<std::string>& _flags);

		//! @brief Default constructor.
		//! @param _flags Intially set flags.
		explicit PropertyBase(PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Assignment constructor.
		//! @param _name Property name.
		//! @param _flags Intially set flags.
		explicit PropertyBase(const std::string& _name, PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Destructor.
		virtual ~PropertyBase() {};

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

		//! @brief Set the property values from the other property.
		//! The base date won't be changed (base class method does nothing).
		//! @param _other Other property.
		void mergeWith(const PropertyBase& _other, const MergeMode& _mergeMode = MergeFlag::MergeValues);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter
		
		void setPropertyName(const std::string& _name) { m_name = _name; };
		const std::string& getPropertyName(void) const { return m_name; };

		//! @brief Set the property title
		//! If no title set the name will be used as title
		void setPropertyTitle(const std::string& _title) { m_title = _title; };

		//! @brief Property title
		//! If no title set the name will be used as title
		const std::string& getPropertyTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		void setPropertyTip(const std::string& _tip) { m_tip = _tip; };
		std::string& getPropertyTip(void) { return m_tip; };
		const std::string& getPropertyTip(void) const { return m_tip; };

		void setPropertyFlag(PropertyFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setPropertyFlags(PropertyFlags _flags) { m_flags = _flags; };
		PropertyFlags& getPropertyFlags(void) { return m_flags; };
		const PropertyFlags& getPropertyFlags(void) const { return m_flags; };

		void setSpecialType(const std::string& _type) { m_specialType = _type; };
		const std::string& getSpecialType(void) const { return m_specialType; };

		void addAdditionalPropertyData(const std::string& _key, const std::string& _data) { m_data.insert_or_assign(_key, _data); };
		void setAdditionalPropertyData(const std::map<std::string, std::string>& _data) { m_data = _data; };
		const std::map<std::string, std::string>& getAllAdditionalPropertyData(void) const { return m_data; };
		std::string getAdditionalPropertyData(const std::string& _key) const;

	private:
		std::string                        m_tip;         //! @brief ToolTip.
		std::string                        m_name;        //! @brief Property name.
		std::string                        m_title;       //! @brief Property title.
		std::string                        m_specialType; //! @brief Special type identifier.
		std::map<std::string, std::string> m_data;        //! @brief Additional data.
		PropertyFlags                      m_flags;       //! @brief Flags.
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::PropertyBase::MergeFlag)
OT_ADD_FLAG_FUNCTIONS(ot::PropertyBase::PropertyFlag)