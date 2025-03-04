//! @file PropertyDirectory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT PropertyDirectory : public Property {
		OT_DECL_NOCOPY(PropertyDirectory)
	public:
		typedef std::string DirectoryValueType;
		using PropertyValueType = DirectoryValueType;

		PropertyDirectory(const PropertyDirectory* _other);
		PropertyDirectory(const PropertyBase& _base);
		PropertyDirectory(PropertyFlags _flags = PropertyFlags(NoFlags)) : Property(_flags) {};
		PropertyDirectory(const std::string& _path, PropertyFlags _flags = PropertyFlags(NoFlags)) : Property(_flags), m_path(_path) {};
		PropertyDirectory(const std::string& _name, const std::string& _path, PropertyFlags _flags = PropertyFlags(NoFlags)) : Property(_name, _flags), m_path(_path) {};
		virtual ~PropertyDirectory() {};

		static std::string propertyTypeString(void) { return "Directory"; };
		virtual std::string getPropertyType(void) const override { return PropertyDirectory::propertyTypeString(); };

		virtual void setValueFromOther(const Property* _other) override;

		virtual Property* createCopy(void) const override;

		void setPath(const std::string& _path) { m_path = _path; };
	
		const std::string& getPath(void) const { return m_path; };

	protected:
		//! @brief Add the property data to the provided JSON object
		//! The property type is already added
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the property data from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setPropertyData(const ot::ConstJsonObject& _object) override;

	private:
		std::string m_path;
	};

}