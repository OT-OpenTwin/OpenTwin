#pragma once

#include <string>

class Result1DData
{
public:
	Result1DData();
	~Result1DData();

	void setDataHashValue(const std::string &dataHash);
	void setData(const std::string &data);

private:
	std::string dataHashValue;
};
