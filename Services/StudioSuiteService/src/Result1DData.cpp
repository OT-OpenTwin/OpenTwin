#include "Result1DData.h"

Result1DData::Result1DData()
{

}

Result1DData::~Result1DData()
{

}

void Result1DData::setDataHashValue(const std::string& dataHash)
{
	dataHashValue = dataHash;
}

void Result1DData::setData(const std::string& data)
{
	std::stringstream dataContent(data);

	// TODO: Here we need to process the data and store the title, axis labels and curve data (real or complex valued)

}

void Result1DData::readLine(std::stringstream& dataContent, std::string& line)
{
	std::getline(dataContent, line);

	if (!line.empty())
	{
		if (line[line.size() - 1] == '\r')
		{
			line.pop_back();
		}
	}
}


