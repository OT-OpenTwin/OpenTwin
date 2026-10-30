// @otlicense
// File: ResultManager.h
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
#include <string>
#include <map>

#include "OTServiceFoundation/BusinessLogicHandler.h"

#include "ParametricCombination.h"

#include "OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h"
#include "OTResultDataAccess/SerialisationInterfaces/ParameterDescription.h"

class MetadataSeries;
class MetadataQuantity;
class EntityFarfieldDump;
class ResultCollectionExtender;

namespace ot
{
	class PainterRainbowIterator;
}

class ResultManager : public BusinessLogicHandler
{
public:
	ResultManager(ot::components::ModelComponent* _modelComponent, const std::string &_tmpDirPath, const std::string& _resultSeriesName, const std::string &_resultBasePath);
	virtual ~ResultManager();

	void clear();
	void loadParameters();
	void convert1D(const std::string &resultName, const std::string &fileName, const std::string& quantityName, const std::string& quantityUnit, const std::string& xAxis, const std::string& xUnit, double xScale, int readDataColumnOnly = -1);
	void convertFarfield(const std::string& fileName, const std::string& frequencyUnit, double frequencyScale, const std::string &excitationString);
	void storeResults(const std::list<std::string> &excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList, const std::map<std::string, std::list<std::string>>& farfieldDumpFrequencies, double structureCx, double structureCy, double structureCz, double structureDiagonal, const std::string& frequencyUnit);

private:
	void addParameterDescriptions(ParametricCombination& currentRun, const std::string& xAxis, const std::string& xUnit, std::list<ot::Variable> parameterValuesXAxis, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void addFarfieldParameterDescriptions(ParametricCombination& currentRun, const std::string& frequencyUnit, double frequencyScale, std::vector<double> &frequency, std::vector<double> &theta, std::vector<double> &phi, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void addCurveData(const std::string& resultName, const std::string& quantityName, const std::string& quantityUnit, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void storeCurves(const std::list<std::string>& excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList, const std::map<std::string, std::list<std::string>>& farfieldDumpFrequencies, double structureCx, double structureCy, double structureCz, double structureDiagonal, const std::string& frequencyUnit);
	bool load1Ddata(const std::string filePath, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, double xScale, int readDataColumnOnly);
		 std::string getPlotName(const std::string& resultName);
	void addFarfieldData(const std::string& excitationString, std::vector<std::complex<double>> &eTheta, std::vector<std::complex<double>> &ePhi, std::vector<double> &eAbs,
	 	 std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void createFarfieldPlots(const std::list<std::string>& excitationStringList, const std::list<EntityFarfieldDump*>& farfieldDumpList, const std::map<std::string, std::list<std::string>>& farfieldDumpFrequencies,
							 const MetadataSeries* series, ResultCollectionExtender &resultCollectionExtender,
							 double structureCx, double structureCy, double structureCz, double structureDiagonal, const std::string& frequencyUnit);
	void createFarfieldPlot(const std::string& type, const std::string& xAxis, const std::string& fixedParameter, const std::string& fixedValue, const std::string& title, const std::string& plotName, const std::list<std::string>& excitationStringList, const MetadataSeries* series, ResultCollectionExtender& resultCollectionExtender);
	void createFarfieldCurve(const std::string& curveName, const std::string& fixedParameter, const std::string& fixedValue, ot::PainterRainbowIterator& rainbowIterator, const MetadataSeries* series, const MetadataQuantity* quantity, ResultCollectionExtender& resultCollectionExtender);
	void createFarfieldPlot3D(const std::string& type, bool complex, const std::string& plotName, EntityFarfieldDump* farfieldDump, const std::list<std::string>& excitationStringList, const MetadataSeries* series, ResultCollectionExtender& resultCollectionExtender,
							  const std::map<std::string, std::list<std::string>>& farfieldDumpFrequencies, double structureCx, double structureCy, double structureCz, double structureDiagonal, const std::string& frequencyUnit);

	std::string tmpDirPath;
	std::string resultBasePath;
	std::string resultSeriesName;
	ParametricCombination parameters;
	std::list<DatasetDescription> allCurveDescriptions;
	std::map<std::string, std::string> plotContainers;
};
