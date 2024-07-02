#pragma once

class Result1DManager;
class Application;
class RunIDContainer;
class Result1DData;
class ResultCollectionExtender;

#include "OTCore/CoreTypes.h"
#include "OTCore/Variable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "MetadataSeries.h"

#include <string>
#include <list>
#include <map>
#include <vector>

struct CurveParameter
{
	std::string parameterName;
	std::string parameterLabel;
	std::string unitName;
	std::vector<ot::Variable> parameterValues;
	std::string valueDataType;
};

class SParameterMatrixHelper
{
public:
	SParameterMatrixHelper(int32_t portNumber)
		: m_portNumber(portNumber)
	{
		m_values.reserve(portNumber);
		for (uint32_t i = 0; i < portNumber; i++)
		{
			std::vector<ot::Variable> zeroInitialised;
			zeroInitialised.reserve(portNumber);
			for (uint32_t i = 0; i < portNumber; i++)
			{
				zeroInitialised.push_back(0.0);
			}
			m_values.push_back(std::move(zeroInitialised));
		}
	}
	void setValue(uint32_t row, uint32_t column, ot::Variable&& value)
	{
		m_values[row][column] = std::move(value);
	}

	ot::Variable& getValue(uint32_t row, uint32_t column)
	{
		return m_values[row][column];
	}

	const int32_t getPortNumber() { return m_portNumber; }

private:
	int32_t m_portNumber = 0;
	std::vector<std::vector<ot::Variable>> m_values;
};

struct DataDescription
{
	virtual ~DataDescription(){}
	std::shared_ptr<std::list<CurveParameter>> parameters = nullptr;

	std::string quantityName = "";
	std::string quantityLabel = "";
	std::string quantityUnit = "";
	uint32_t quantityDataNumberOfRows = 0;
	uint32_t quantityDataNumberOfColumns = 0;
};

struct CurveDescription : public DataDescription
{
	CurveDescription()
	{
		quantityDataNumberOfRows = 1;
		quantityDataNumberOfColumns = 1;
	}
	std::vector<ot::Variable> quantityValues;
};

struct SParameterDescription : public DataDescription
{	
	std::vector<SParameterMatrixHelper> quantityValues;
};

class ParametricResult1DManager : public BusinessLogicHandler
{
public:
	ParametricResult1DManager(Application* app);
	ParametricResult1DManager() = delete;
	~ParametricResult1DManager();

	void clear();
	void add(Result1DManager& result1DManager);

private:
	std::string determineRunIDLabel(std::list<int>& runIDList);
	
	std::list<CurveDescription> extractCurveDescriptions(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	std::list<SParameterDescription> extractSParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	std::shared_ptr<std::list<CurveParameter>> extractCurveParameters(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	void extractMetadataFromDescriptions(std::map<std::string, MetadataQuantity>& _quantitiesByName, std::map<std::string, MetadataParameter>& _parametersByName, std::list<DataDescription*> _allDataDescriptions);
	void processDataPoints(DataDescription* dataDescription, ResultCollectionExtender& _resultCollectionExtender, std::string& _seriesName);

	MetadataQuantity createNewQuantity(DataDescription* _dataDescription);
	MetadataParameter createNewParameter(CurveParameter& _curveParameter);

	int determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources);

	void parseAxisLabel(const std::string& value, std::string& label, std::string& unit);

	std::string m_resultFolderName;
	std::string m_seriesNameBase = "CST Imported Results";
	std::string m_parameterNameRunID = "Run ID";
	Application* m_application;
};
