// @otlicense

#pragma once

// project header

// std header
#include <string>
#include <list>

class Parameter {
public:
	enum DataType {
		Unknown,
		Boolean,
		Char,
		Integer,
		Float,
		Double,
		String,
		Array,
		Object,
		Enum,
		UnsignedInteger64
	};

	//Parameter() = default;
	Parameter();
	Parameter(const Parameter& _other) = default;
	Parameter(Parameter&& _other) noexcept = default;

	~Parameter() = default;

	Parameter& operator = (const Parameter& _other) = default;
	Parameter& operator = (Parameter&& _other) noexcept = default;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setDataType(DataType _dataType) { m_dataType = _dataType; };
	DataType getDataType() const { return m_dataType; };
	std::string getDataTypeString() const;

	void addDescription(const std::string& _description) { m_description.push_back(_description); };
	const std::list<std::string>& getDescription() const { return m_description; };
	void printDescription() const;

	void setMacro(const std::string& _macro) { m_macro = _macro; };
	const std::string& getMacro() const { return m_macro; }

	void printParameter() const;

private:
	std::string m_name;
	DataType m_dataType;
	std::list<std::string> m_description;
	std::string m_macro;
};