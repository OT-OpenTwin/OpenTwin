//! @file BlockConfigurationObject.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {
	
	class __declspec(dllexport) BlockConfigurationObject : public ot::Serializable {
	public:
		BlockConfigurationObject() {};
		BlockConfigurationObject(const std::string& _name) : m_name(_name) {};
		BlockConfigurationObject(const BlockConfigurationObject& _other) : m_name(_other.m_name) {};
		virtual ~BlockConfigurationObject() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

	private:
		std::string m_name;

	};

}