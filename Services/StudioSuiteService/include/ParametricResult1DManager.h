#pragma once

class Result1DManager;
class Application;
class RunIDContainer;
class Result1DData;
class ResultCollectionExtender;


#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "MetadataSeries.h"
#include "DataDescription1D.h"

#include <string>
#include <list>
#include <map>
#include <vector>


class ParametricResult1DManager : public BusinessLogicHandler
{
public:
	ParametricResult1DManager(Application* app);
	ParametricResult1DManager() = delete;
	~ParametricResult1DManager();

	void clear();
	void extractData(Result1DManager& result1DManager);
	void storeDataInResultCollection();
private:
	std::string determineRunIDLabel(std::list<int>& runIDList);
	
	std::list<DataDescription1D> extractDataDescriptionCurve(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	std::list<DataDescription1D> extractDataDescriptionSParameter(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	std::list<std::shared_ptr<ParameterDescription>> extractParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	void extractMetadataFromDescriptions(std::map<std::string, MetadataQuantity>& _quantitiesByName, std::map<std::string, MetadataParameter>& _parametersByName, std::list<DataDescription1D>& _allDataDescriptions);
	void addParameterMetadata(std::map<std::string, MetadataParameter>& _parametersByName, std::shared_ptr<ParameterDescription> _parameter);
	void processDataPoints(DataDescription1D& dataDescription, ResultCollectionExtender& _resultCollectionExtender, std::string& _seriesName);

	MetadataQuantity createNewQuantity(const QuantityDescription* const _quantityDescription);
	MetadataParameter createNewParameter(const ParameterDescription* const _paramParameter);

	int determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources);

	void parseAxisLabel(const std::string& value, std::string& label, std::string& unit);

	std::list<DataDescription1D> m_allDataDescriptions;

	std::string m_resultFolderName;
	std::string m_seriesNameBase = "CST Imported Results";
	std::string m_parameterNameRunID = "Run ID";
	Application* m_application;
};
