#pragma once
#include "BusinessLogicHandler.h"

#include "TouchstoneHandler.h"
#include "MetadataSeries.h"

class TouchstoneToResultdata : public BusinessLogicHandler
{
public:
	TouchstoneToResultdata();
	~TouchstoneToResultdata();
	void CreateResultdata(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength);
private:
	std::string _collectionName = "";

	const std::string CreateSeriesName(const std::string& fileName);
	bool SeriesAlreadyExists(const std::string& seriesName);

	TouchstoneHandler ImportTouchstoneFile(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength);
	void BuildSeriesMetadataFromTouchstone(TouchstoneHandler& touchstoneHandler, MetadataSeries& series);
	//std::string DetermineDominantType(const std::list<std::string>& typeNames);
};

