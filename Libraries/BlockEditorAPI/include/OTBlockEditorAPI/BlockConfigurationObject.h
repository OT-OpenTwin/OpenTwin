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

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

	private:
		std::string m_name;

	};

}