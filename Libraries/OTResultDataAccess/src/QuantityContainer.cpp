// @otlicense
// File: QuantityContainer.cpp
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

// OpenTwin header
#include "OTModelEntities/VariableToBSONConverter.h"
#include "OTResultDataAccess/MetadataSeries.h"
#include "OTResultDataAccess/MetadataQuantity.h"
#include "OTResultDataAccess/QuantityContainer.h"

QuantityContainer::QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterAbbreviations, std::list<ot::Variable>&& _parameterValues, int64_t _quantityIndex)
{
	m_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataSeries::getFieldName(), _seriesIndex));
	m_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataQuantity::getFieldName(), _quantityIndex));

	VariableToBSONConverter converter;
	auto parameterValue = _parameterValues.begin();
	for (const auto& parameterAbbreviation : _parameterAbbreviations)
	{
		converter(m_mongoDocument, *parameterValue++, std::to_string(parameterAbbreviation));
	}
}

QuantityContainer::QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterAbbreviations, std::list<ot::Variable>& _parameterValues, int64_t _quantityIndex)
{
	m_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataSeries::getFieldName(), _seriesIndex));
	m_mongoDocument.append(bsoncxx::builder::basic::kvp(MetadataQuantity::getFieldName(), _quantityIndex));

	VariableToBSONConverter converter;
	auto parameterValue = _parameterValues.begin();
	for (const auto& parameterAbbreviation : _parameterAbbreviations)
	{
		converter(m_mongoDocument, *parameterValue++, std::to_string(parameterAbbreviation));
	}

}

QuantityContainer::QuantityContainer(QuantityContainer&& other) noexcept
: m_values(std::move(other.m_values)), m_mongoDocument (std::move(other.m_mongoDocument))
{
	
	
}


QuantityContainer& QuantityContainer::operator=(QuantityContainer&& _other)
{
	m_values = std::move(_other.m_values);
	m_mongoDocument = std::move(_other.m_mongoDocument);
	return *this;
}

QuantityContainer::~QuantityContainer()
{
}

void QuantityContainer::addValue(const ot::Variable& _value)
{
	m_values.push_back({ _value });
}

void QuantityContainer::addValue(const std::list<ot::Variable>& _values)
{
	m_values.push_back(_values);
}

bsoncxx::builder::basic::document& QuantityContainer::getMongoDocument()
{
	// If tuples are used, we first guarantee that all entries are tuples of the same size
	size_t tupleSize = m_values.front().size();
	for (const auto& value : m_values)
	{
		for (const auto& entry : value)
		{
			assert(value.size() == tupleSize);
		}
	}

	VariableToBSONConverter converter;
	// First we distinguish between matrices/vectors and single points
	if (m_values.size() == 1)
	{
		auto singleEntry = m_values.begin();
		//Now we distinguish between tuples and non-tuples
		if(singleEntry->size() == 1)
		{
			converter(m_mongoDocument, singleEntry->front(), QuantityContainer::getFieldName());
		}
		else
		{
			auto tupleArray = bsoncxx::builder::basic::array();
			for (auto& value : *singleEntry)
			{
				converter(tupleArray, value);
			}
			m_mongoDocument.append(bsoncxx::builder::basic::kvp(QuantityContainer::getFieldName(), tupleArray));
		}
	}
	else
	{
		auto valueArray = bsoncxx::builder::basic::array();
		for (auto& value : m_values)
		{
			//Now we distinguish between tuples and non-tuples
			if (value.size() == 1)
			{
				converter(valueArray, value.front());				
			}
			else
			{
				auto tupleArray = bsoncxx::builder::basic::array();
				for (auto& tupleEntry: value)
				{
					converter(tupleArray, tupleEntry);
				}
				valueArray.append(tupleArray);
			}		
		}
		m_mongoDocument.append(bsoncxx::builder::basic::kvp(QuantityContainer::getFieldName(), valueArray));
	}
	return m_mongoDocument;
}

