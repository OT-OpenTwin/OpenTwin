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

// OpenTwin header
#include "OTGui/QuantityContainerEntryDescription.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTResultDataAccess/CurveFactory.h"
#include "OTResultDataAccess/QuantityContainer.h"
#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"	

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config)
{
	addToConfig(_series, _config, "", "", "");
}

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config, const std::string& _quantityNameOnYAxis, const std::string& _quantityValueDescriptionNameOnYAxis, const std::string& _defaultParameterForXAxis)
{
	ot::QueryInformation queryInformation;
	queryInformation.m_projection = createProjection();

	const std::list<MetadataQuantity>& quantities = _series.getQuantities();

	//assert(quantities.size() == 1);
	
	const MetadataQuantity* selectedQuantity = nullptr;
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
	
	auto& tupleDescription = selectedQuantity->m_tupleDescription;
	int tupleIndex = 0;
	
	if (!_quantityValueDescriptionNameOnYAxis.empty())
	{
		for (auto& tupleElementName : tupleDescription->getTupleElementNames())
		{
			if (tupleElementName == _quantityValueDescriptionNameOnYAxis)
			{
				break;
			}
			else
			{
				tupleIndex++;
				if (tupleIndex >= tupleDescription->getTupleElementNames().size())
				{
					throw std::invalid_argument("Creating a curve failed to extract the y-axis information. The provided value description name does not match any of the value descriptions of the quantity.");
				}
			}	
		}
	}
		
	queryInformation.m_query = createQuery(_series.getSeriesIndex(), selectedQuantity->quantityIndex);

	ot::QuantityContainerEntryDescription quantityInformation;
	quantityInformation.m_fieldName = QuantityContainer::getFieldName();
	quantityInformation.m_label = selectedQuantity->quantityName;
	quantityInformation.m_unit = tupleDescription->getUnits()[tupleIndex];
	quantityInformation.m_dataType = tupleDescription->getDataType();
	quantityInformation.m_dimension = selectedQuantity->dataDimensions;
	queryInformation.m_quantityDescription = quantityInformation;

	const std::list<MetadataParameter>& parameters = _series.getParameter();
	auto& dependingParameter = selectedQuantity->dependingParameterIds;
	for (const auto& parameter : parameters)
	{
		if (std::find(dependingParameter.begin(), dependingParameter.end(), parameter.parameterUID) != dependingParameter.end())
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
	}

	_config.setQueryInformation(queryInformation);

}

const std::string CurveFactory::createQuery(ot::UID _seriesID, ot::UID _quantityID)
{
	AdvancedQueryBuilder builder;
	ot::ValueComparisonDefinition seriesComparison(MetadataSeries::getFieldName(), "=", std::to_string(_seriesID), ot::TypeNames::getInt64TypeName(), "");
	auto firstComparision = builder.createComparison(seriesComparison);

	ot::ValueComparisonDefinition quantityComparison(MetadataQuantity::getFieldName(), "=", std::to_string(_quantityID), ot::TypeNames::getInt64TypeName(), "");
	auto secondComparision = builder.createComparison(quantityComparison);
	auto finalQuery = builder.connectWithAND({ firstComparision,secondComparision });

	const std::string debugQuery = bsoncxx::to_json(finalQuery.view());
	return debugQuery;
}

const std::string CurveFactory::createProjection()
{
	AdvancedQueryBuilder builder;
	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType", MetadataSeries::getFieldName() };
	const std::string projection = bsoncxx::to_json(builder.GenerateSelectQuery(projectionNamesForExclusion, false, false));
	return projection;
}
