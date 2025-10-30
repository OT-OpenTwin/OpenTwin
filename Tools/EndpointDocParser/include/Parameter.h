// @otlicense
// File: Parameter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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