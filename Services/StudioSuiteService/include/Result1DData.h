#pragma once

#include <string>
#include <sstream>

class Result1DData
{
public:
	Result1DData();
	~Result1DData();

	void setDataHashValue(const std::string &dataHash);
	void setData(const std::string &data);

	std::string getDataHashValue() { return dataHashValue; }

private:
	void readLine(std::stringstream& dataContent, std::string& line);

	std::string dataHashValue;
};
