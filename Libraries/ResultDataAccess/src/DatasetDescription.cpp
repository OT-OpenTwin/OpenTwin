// @otlicense
// File: DatasetDescription.cpp
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

#include "DatasetDescription.h"
#include <assert.h>

DatasetDescription::~DatasetDescription()
{
	if (m_quantityDescription != nullptr)
	{
		delete m_quantityDescription;
		m_quantityDescription = nullptr;
	}
}

DatasetDescription::DatasetDescription()
{

}

DatasetDescription& DatasetDescription::operator=(DatasetDescription&& other) noexcept
{
	this->m_parameters = std::move(other.m_parameters);
	this->m_quantityDescription = other.m_quantityDescription;
	other.m_quantityDescription = nullptr;
	return *this;
}

DatasetDescription::DatasetDescription(DatasetDescription&& other) noexcept
	: m_parameters(std::move(other.m_parameters)), m_quantityDescription(std::move(other.m_quantityDescription))
{
	other.m_quantityDescription = nullptr;
}

void DatasetDescription::setQuantityDescription(QuantityDescription* _quantityDescription)
{
	m_quantityDescription = _quantityDescription;
}

void DatasetDescription::addParameterDescription(std::shared_ptr<ParameterDescription> _parameterDescription)
{
	assert(_parameterDescription != nullptr);
	m_parameters.push_back(_parameterDescription);
}

void DatasetDescription::addParameterDescriptions(std::list<std::shared_ptr<ParameterDescription>>_parameterDescriptions)
{
	m_parameters.insert(m_parameters.end(), _parameterDescriptions.begin(), _parameterDescriptions.end());
}

