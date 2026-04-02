// @otlicense
// File: CurveDatasetFactory.h
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

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTCore/Variable/Variable.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTDataStorage/DataLakeAPI.h"
#include "Datapoints.h"
#include "AdditionalDependencies.h"

// std header
#include <vector>
#include <string>
#include <complex>

class CurveDatasetFactory
{
public:
	CurveDatasetFactory(const std::string& _collectionName)
		: m_dataAccess(_collectionName){ }
	
	std::list<ot::PlotDataset*> createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg);

	const std::list<std::string>& getCurveIDDescriptions() { return m_curveIDDescriptions; }
private:

	enum class CurveType : uint32_t
	{
		m_single = 0,
		m_familyCurve = 1
	};

	std::list<std::string> m_curveIDDescriptions;
	std::list<std::string> m_skippedValueComparisons;
	DataStorageAPI::DataLakeAPI m_dataAccess;

	std::string createUnitLabel(const std::string& _unit);
	
	std::unordered_map<DependencyList, std::list<Datapoints>>  createCurves(ot::Plot1DCfg& _plotCfg, ot::Plot1DCurveCfg& _curveCfg, ot::ConstJsonArray& _allMongoDBDocuments);
	std::map<std::string, std::list<Datapoints>>  createNamedCurveFamilies(std::unordered_map<DependencyList, std::list<Datapoints>>& _datasetsByDependencies, ot::Plot1DCurveCfg& _curveCfg);
	std::list<ot::PlotDataset*> createPlotDatasets(std::map<std::string, std::list<Datapoints>>& _curvesByCurveTitle, ot::Plot1DCurveCfg& _curveCfg);

	double jsonToDouble(const std::string& _memberName, ot::ConstJsonObject& _jesonEntry, const std::string& _dataType);
	double jsonToDouble(const rapidjson::Value& _jesonEntry, const std::string& _dataType);
	double jsonToDouble(const rapidjson::Value& _jesonEntry);

	ot::PlotDatasetData createCurveData(const std::vector<double>& _xData, const std::vector<ot::Variable>& _yData, bool _yDataIsComplex, const ot::Plot1DCfg& _plotCfg);
	static std::vector<double> toDoubleVector(const std::vector<ot::Variable>& _values);
	static std::vector<std::complex<double>> toComplexVector(const std::vector<ot::Variable>& _values);
};
