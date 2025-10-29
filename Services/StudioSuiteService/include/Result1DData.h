// @otlicense

#pragma once

#include <string>
#include <sstream>
#include <vector>

class Result1DData
{
public:
	Result1DData();
	~Result1DData();

	void setDataHashValue(const std::string &dataHash);
	void setData(const std::string &data);

	std::string getDataHashValue() { return dataHashValue; }
	int getNumberOfQuantities();

	std::string getTitle() { return title; }
	std::string getXLabel() { return xlabel; }
	std::string getYLabel() { return ylabel; }
	std::vector<double>& getXValues() { return xValues; }
	std::vector<double>& getYreValues() { return yreValues; }
	std::vector<double>& getYimValues() { return yimValues; }

private:
	void readLine(std::stringstream& dataContent, std::string& line);
	void readDataLine(std::stringstream& dataContent, double& x, double& yre, double& yim, bool& xRead, bool& yreRead, bool& yimRead);

	std::string dataHashValue;
	std::string title;
	std::string xlabel;
	std::string ylabel;
	std::vector<double> xValues;
	std::vector<double> yreValues;
	std::vector<double> yimValues;
};
