//! @file PropertyStringList.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Property.h"

// std header
#include <list>
#include <string>
#include <vector>

#define OT_PROPERTY_TYPE_StringList "StringList"

namespace ot {

	class OT_CORE_API_EXPORT PropertyStringList : public Property {
		OT_DECL_NOCOPY(PropertyStringList)
	public:
		PropertyStringList(PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyStringList(const std::string& _current, PropertyFlags _flags = PropertyFlags::NoFlags) ;
		PropertyStringList(const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyStringList(const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyStringList(const std::string& _name, const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags = PropertyFlags::NoFlags);
		PropertyStringList(const std::string& _name, const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags = PropertyFlags::NoFlags);
		virtual ~PropertyStringList() {};

		virtual std::string getPropertyType(void) const override { return OT_PROPERTY_TYPE_StringList; };

		virtual Property* createCopy(void) const override;

		void setList(const std::list<std::string>& _values) { m_list = _values; };
		std::list<std::string>& list(void) { return m_list; };
		const std::list<std::string>& list(void) const { return m_list; };

		void setCurrent(const std::string& _current) { m_current = _current; };
		const std::string& current(void) const { return m_current; };

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
		std::string m_current;
		std::list<std::string> m_list;
	};

}