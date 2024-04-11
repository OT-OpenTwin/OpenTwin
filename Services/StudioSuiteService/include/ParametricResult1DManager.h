#pragma once

class Result1DManager;
class Application;
class RunIDContainer;
class Result1DData;
class ResultCollectionExtender;

#include "OTCore/CoreTypes.h"
#include "OTCore/Variable.h"

#include <string>
#include <list>
#include <map>
#include <vector>

class ParametricResult1DManager
{
public:
	ParametricResult1DManager(Application* app);
	ParametricResult1DManager() = delete;
	~ParametricResult1DManager();

	void clear();
	void add(Result1DManager& result1DManager);

private:
	std::string determineRunIDLabel(std::list<int>& runIDList);
	void processCurves(const std::string& category, const std::string& runIDLabel, std::list<int>& runIDList, Result1DManager& result1DManager, ResultCollectionExtender& resultCollectionExtender);
	void processSparameters(const std::string& category, const std::string& runIDLabel, std::list<int>& runIDList, Result1DManager& result1DManager, ResultCollectionExtender& resultCollectionExtender);
	void parseAxisLabel(const std::string& value, std::string& label, std::string& unit);
	void addCurveSeriesMetadata(ResultCollectionExtender& resultCollectionExtender, const std::string& category, const std::string& seriesName,  
								const std::string& xLabel, const std::string& xUnit, RunIDContainer* container,
								std::map<std::string, Result1DData*>& categoryResults, std::list<ot::Variable>& xValues, bool hasRealPart, bool hasImagPart, size_t numberOfQuantities);
	void addCurveSeriesQuantityData(ResultCollectionExtender& resultCollectionExtender, const std::string& seriesName, const std::string& xLabel, size_t numberOfXValues,
									std::map<std::string, Result1DData*>& categoryResults, bool hasRealPart, bool hasImagPart);
	void addSparameterSeriesMetadata(ResultCollectionExtender& resultCollectionExtender, const std::string& category, const std::string& seriesName,
									 const std::string& xLabel, const std::string& xUnit, RunIDContainer* container, int numberPorts,
									 std::map<std::string, Result1DData*>& categoryResults, std::list<ot::Variable>& xValues);
	void addSparameterSeriesQuantityData(ResultCollectionExtender& resultCollectionExtender, const std::string& seriesName, const std::string& xLabel, size_t numberOfXValues,
									     std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*> &sources);
	int determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources);

	std::string resultFolderName;
	Application* application;
};
