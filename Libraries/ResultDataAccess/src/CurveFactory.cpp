// @otlicense
// File: CurveFactory.cpp
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

#include "CurveFactory.h"
#include "QuantityContainer.h"
#include "OTGui/QuantityContainerEntryDescription.h"
#include "AdvancedQueryBuilder.h"

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config)
{
	addToConfig(_series, _config, "", "", "");
}

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config, const std::string& _quantityNameOnYAxis, const std::string& _quantityValueDescriptionNameOnYAxis, const std::string& _defaultParameterForXAxis)
{
	ot::QueryInformation queryInformation;
	queryInformation.m_query = createQuery(_series.getSeriesIndex());
	queryInformation.m_projection = createProjection();

	const std::list<MetadataQuantity>& quantities = _series.getQuantities();

	assert(quantities.size() == 1);
	
	const MetadataQuantity* selectedQuantity = nullptr;
	const MetadataQuantityValueDescription* selectedValueDescription = nullptr;
	if (!_quantityNameOnYAxis.empty())
	{
		for (const MetadataQuantity& quantity : quantities)
		{
			if (quantity.quantityName == _quantityNameOnYAxis)
			{
				selectedQuantity = &quantity;
				break;
			}
		}
	}
	else
	{
		if (quantities.size() > 1)
		{
			throw std::invalid_argument("Creating a curve is only possible with a single quantity");
		}
		selectedQuantity = &(*quantities.begin());
	}

	if (selectedQuantity == nullptr)
	{
		throw std::exception("Curve creation failed to extract the y-axis information");
	}
	
	if (!_quantityValueDescriptionNameOnYAxis.empty())
	{
		for (auto& valueDescription : selectedQuantity->valueDescriptions)
		{
			if (valueDescription.quantityValueLabel == _quantityValueDescriptionNameOnYAxis)
			{
				selectedValueDescription = &valueDescription;
				break;
			}
		}
	}
	else
	{
		selectedValueDescription = &(*selectedQuantity->valueDescriptions.begin());
	}
	
	if (selectedValueDescription == nullptr)
	{
		throw std::exception("Curve creation failed to extract the y-axis information");
	}

	ot::QuantityContainerEntryDescription quantityInformation;
	quantityInformation.m_fieldName = QuantityContainer::getFieldName();
	quantityInformation.m_label = selectedQuantity->quantityName + selectedValueDescription->quantityValueName;
	quantityInformation.m_unit = selectedValueDescription->unit;
	quantityInformation.m_dataType = selectedValueDescription->dataTypeName;
	quantityInformation.m_dimension = selectedQuantity->dataDimensions;
	queryInformation.m_quantityDescription = quantityInformation;

	const std::list<MetadataParameter>& parameters = _series.getParameter();

	for (const auto& parameter : parameters)
	{
		ot::QuantityContainerEntryDescription qcDescription;
		qcDescription.m_label = parameter.parameterLabel;
		qcDescription.m_dataType = parameter.typeName;
		qcDescription.m_fieldName = std::to_string(parameter.parameterUID);
		qcDescription.m_unit = parameter.unit;
		if (!_defaultParameterForXAxis.empty() && _defaultParameterForXAxis == parameter.parameterName)
		{
			queryInformation.m_parameterDescriptions.push_front(qcDescription);
		}
		else
		{
			queryInformation.m_parameterDescriptions.push_back(qcDescription);
		}
	}

	_config.setQueryInformation(queryInformation);

}

const std::string CurveFactory::createQuery(ot::UID _seriesID)
{
	const std::string query = "{\"" + MetadataSeries::getFieldName() + "\":" + std::to_string(_seriesID) + "}";
	return query;
}

const std::string CurveFactory::createProjection()
{
	AdvancedQueryBuilder builder;
	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType", MetadataSeries::getFieldName() };
	const std::string projection = bsoncxx::to_json(builder.GenerateSelectQuery(projectionNamesForExclusion, false, false));
	return projection;
}
