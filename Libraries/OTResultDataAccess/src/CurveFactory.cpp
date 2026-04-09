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
#include "OTResultDataAccess/DataLakeAccessor.h"

void CurveFactory::addToConfig(const MetadataSeries& _series, const MetadataQuantity& _quantity, ot::Plot1DCurveCfg& _config, ot::ApplicationBase* _appBase, ResultCollectionMetadataAccess* _access)
{
	DataLakeAccessor dataLakeAccessor(_appBase);
	const std::string collectionName = _appBase->getCollectionName();

	if (_access == nullptr)
	{
		dataLakeAccessor.accessPartition(collectionName);
	}
	else
	{
		dataLakeAccessor.accessPartition(_access);
	}
	
	// No series metadata query necessary, just the limitation to a single series
	const std::string seriesLabel = _series.getName();
	
	dataLakeAccessor.createQueryDescriptionsSeries({}, seriesLabel);

	DataLakeQueryCfg queryCfg;
	queryCfg.setCollectionName(_appBase->getProjectName());
	queryCfg.setSeriesLabel(seriesLabel);

	// Only the selection the quantity necessary
	ot::ValueComparisonDescription quantitySelection(_quantity.quantityLabel, "", "", _quantity.m_tupleDescription);
	dataLakeAccessor.createQueryDescriptionQuantity(quantitySelection);
	queryCfg.setValueDescriptionQuantities(quantitySelection);


	const std::list<MetadataParameter>& allParameter = _series.getParameter();
	std::list<std::string> dependendParameterLabels; 
	
	for (const MetadataParameter& parameter : allParameter)
	{
		if (std::find(_quantity.dependingParameterIds.begin(), _quantity.dependingParameterIds.end(), parameter.parameterUID)  != _quantity.dependingParameterIds.end())
		{
			dependendParameterLabels.push_back(parameter.parameterLabel);
		}
	}

	queryCfg.setParameterOptions({ dependendParameterLabels.begin(), dependendParameterLabels.end() });

	ot::DataLakeAccessCfg config = dataLakeAccessor.createConfig();
	_config.setDataAccessConfig(std::move(config));
	_config.setQueryCfg(queryCfg);
}

//void CurveFactory::addToConfig(ot::Plot1DCurveCfg& _config, ot::ApplicationBase* _appBase, const std::string& _seriesLabel)
//{
//	DataLakeAccessor dataLakeAccessor(_appBase);
//	const std::string collectionName = _appBase->getCollectionName();
//	dataLakeAccessor.accessPartition(collectionName);
//
//	// No series metadata query necessary, just the limitation to a single series
//	dataLakeAccessor.createQueryDescriptionsSeries({}, _seriesLabel);
//
//	DataLakeQueryCfg queryCfg;
//	queryCfg.setCollectionName(collectionName);
//	queryCfg.setSeriesLabel(_seriesLabel);
//
//	ot::DataLakeAccessCfg config = dataLakeAccessor.createConfig();
//	_config.setDataAccessConfig(std::move(config));
//	_config.setQueryCfg(queryCfg);
//}
