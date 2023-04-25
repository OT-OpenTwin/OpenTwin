#pragma once

// C++ header
#include <string>

class ServiceInformation {
public:
	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type);
	ServiceInformation(const ServiceInformation& _other);
	virtual ~ServiceInformation();

	ServiceInformation& operator = (const ServiceInformation& _other);
	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	void setName(const std::string& _name) { m_name = _name; }
	const std::string& name(void) const { return m_name; }

	void setType(const std::string& _type) { m_type = _type; }
	const std::string& type(void) const { return m_type; }

private:
	std::string		m_name;
	std::string		m_type;
};