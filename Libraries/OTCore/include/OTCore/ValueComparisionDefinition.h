#pragma once
#include <string>
#include "OTCore/Serializable.h"

class __declspec(dllexport) ValueComparisionDefinition : public ot::Serializable
{
public:
	ValueComparisionDefinition(const std::string& _name, const std::string& _comparator, const std::string& _value, const std::string& _type, const std::string& _unit)
		: m_name(_name), m_comparator(_comparator), m_value(_value), m_type(_type) {
	}
	ValueComparisionDefinition() = default;
	const std::string& getName() const { return m_name; }
	const std::string& getComparator() const { return m_comparator; }
	const std::string& getValue() const { return m_value; }
	const std::string& getType() const { return m_type; }
	const std::string& getUnit() const { return m_unit; }

	void setName(const std::string& name) { m_name = name; }
	void setType(const std::string& _type) { m_type = _type; }
	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
	void setFromJsonObject(const ot::ConstJsonObject& _object) override;

private:
	std::string m_name;
	std::string m_comparator;
	std::string m_value;
	std::string m_type;
	std::string m_unit;
};
