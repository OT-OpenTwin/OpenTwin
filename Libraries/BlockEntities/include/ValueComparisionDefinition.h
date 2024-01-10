#pragma once
#include <string>

class __declspec(dllexport) ValueComparisionDefinition
{
public:
	ValueComparisionDefinition(const std::string& name, const std::string& comparator, const std::string& value, const std::string& type)
		: _name(name), _comparator(comparator), _value(value), _type(type){}
	const std::string& getName() const { return _name; }
	const std::string& getComparator() const { return _comparator; }
	const std::string& getValue() const { return _value; }
	const std::string& getType() const { return _type; }
	void setName(const std::string& name) { _name = name; }
private:
	std::string _name;
	const std::string _comparator;
	const std::string _value;
	const std::string _type;
};
