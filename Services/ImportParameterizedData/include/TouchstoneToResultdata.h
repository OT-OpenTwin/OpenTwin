#pragma once
#include "BusinessLogicHandler.h"
#include "ResultCollectionExtender.h"

#include "TouchstoneHandler.h"
#include "MetadataSeries.h"
#include "DatasetDescription1D.h"

class TouchstoneToResultdata : public BusinessLogicHandler
{
public:
	TouchstoneToResultdata();
	~TouchstoneToResultdata();
	int getAssumptionOfPortNumber(const std::string& fileName);
	void setResultdata(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength);
	void createResultdata(int numberOfPorts);

private:
	std::string m_collectionName = "";
	std::string m_fileName = "";
	std::string m_fileContent = "";
	uint64_t m_uncompressedLength = 0;

	const std::string createSeriesName(const std::string& fileName);
	bool seriesAlreadyExists(const std::string& seriesName);

	TouchstoneHandler importTouchstoneFile(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength, int numberOfPorts);
	DatasetDescription1D extractDatasetDescription(TouchstoneHandler& _handler);
};

