// Project header
#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() {

}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type)
	: m_name(_name), m_type(_type)
{}

ServiceInformation::ServiceInformation(const ServiceInformation& _other) 
	: m_name(_other.m_name), m_type(_other.m_type)
{}

ServiceInformation::~ServiceInformation() {}

ServiceInformation& ServiceInformation::operator = (const ServiceInformation& _other) {
	m_name = _other.m_name;
	m_type = _other.m_type;

	return *this;
}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type;
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}