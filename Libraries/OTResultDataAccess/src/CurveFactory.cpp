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
#include "OTCore/QueryDescription/DataPointDecoder.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTResultDataAccess/CurveFactory.h"
#include "OTResultDataAccess/QuantityContainer.h"
#include "OTCore/Tuple/TupleDescription.h"	
#include "OTCore/Tuple/TupleFactory.h"
#include "OTCore/Tuple/TupleDescriptionSingle.h"

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config)
{
	addToConfig(_series, _config, "", "", "");
}

void CurveFactory::addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config, const std::string& _quantityNameOnYAxis, const std::string& _quantityValueDescriptionNameOnYAxis, const std::string& _defaultParameterForXAxis)
{
	

}

const std::string CurveFactory::createQuery(ot::UID _seriesID, ot::UID _quantityID)
{
	AdvancedQueryBuilder builder;
	ot::ValueComparisonDescription seriesComparison(MetadataSeries::getFieldName(), "=", std::to_string(_seriesID), ot::TypeNames::getInt64TypeName(), "");
	auto firstComparision = builder.createComparison(seriesComparison);

	ot::ValueComparisonDescription quantityComparison(MetadataQuantity::getFieldName(), "=", std::to_string(_quantityID), ot::TypeNames::getInt64TypeName(), "");
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
