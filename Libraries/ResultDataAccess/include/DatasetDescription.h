// @otlicense
// File: DatasetDescription.h
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

#include <list>
#include <memory>

#include "QuantityDescription.h"
#include "ParameterDescription.h"

class __declspec(dllexport) DatasetDescription
{
public:
	virtual ~DatasetDescription();
	DatasetDescription();
	DatasetDescription& operator=(DatasetDescription&& other) noexcept;
	DatasetDescription(DatasetDescription&& other) noexcept;
	DatasetDescription& operator=(const DatasetDescription& other) = delete;
	DatasetDescription(const DatasetDescription& other) = delete;

	const std::list<std::shared_ptr<ParameterDescription>>& getParameters() const { return m_parameters; }
	QuantityDescription* getQuantityDescription() { return m_quantityDescription; }

	void setQuantityDescription(QuantityDescription* _quantityDescription);
	
	void addParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription);
	void addParameterDescriptions(std::list<std::shared_ptr<ParameterDescription>>_parameterDescriptions);
protected:

	std::list<std::shared_ptr<ParameterDescription>> m_parameters;
	QuantityDescription* m_quantityDescription = nullptr;
};
