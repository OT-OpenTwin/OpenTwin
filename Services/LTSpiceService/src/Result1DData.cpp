#include "Result1DData.h"

#include <algorithm>
#include <cassert>

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

	readLine(dataContent, title);
	readLine(dataContent, xlabel);
	readLine(dataContent, ylabel);

	std::string line;
	readLine(dataContent, line);
	size_t numberOfDataPoints = atoll(line.c_str());

	xValues.clear();
	yreValues.clear();
	yimValues.clear();

	for (size_t index = 0; index < numberOfDataPoints; index++)
	{
		double x(0), yre(0), yim(0);
		bool xRead(false), yreRead(false), yimRead(false);

		readDataLine(dataContent, x, yre, yim, xRead, yreRead, yimRead);

		if (xRead && xValues.capacity() == 0)
		{
			xValues.reserve(numberOfDataPoints);
		}

		if (yreRead && yreValues.capacity() == 0)
		{
			yreValues.reserve(numberOfDataPoints);
		}

		if (yimRead && yimValues.capacity() == 0)
		{
			yimValues.reserve(numberOfDataPoints);
		}

		if (xRead)
		{
			xValues.push_back(x);
		}

		if (yreRead)
		{
			yreValues.push_back(yre);
		}

		if (yimRead)
		{
			yimValues.push_back(yim);
		}
	}
}

void Result1DData::readDataLine(std::stringstream& dataContent, double& x, double& yre, double& yim, bool& xRead, bool& yreRead, bool& yimRead)
{
	std::string line;
	readLine(dataContent, line);

	// The format of the line depends on the type of data
	// Real valued:    (0.0011740265181288123, -7.56392637413228e-09), d.h. (x, yre)
	// Complex valued: (8.0, (-0.051995255053043365+0.11043525487184525j)), d.h. (x, (yre+yimj))

	// First we split the line into the individual strings
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	line.erase(std::remove(line.begin(), line.end(), '('), line.end());
	line.erase(std::remove(line.begin(), line.end(), ')'), line.end());

	xRead = yreRead = yimRead = false;
	x = yre = yim = 0.0;

	size_t index = line.find(',');

	if (index == line.npos)
	{
		// We only have a y value
		yre = atof(line.c_str());
		yreRead = true;
	}
	else
	{
		std::string xValue = line.substr(0, index);
		std::string yValue = line.substr(index + 1);

		x = atof(xValue.c_str());
		xRead = true;

		if (yValue[yValue.size() - 1] == 'j')
		{
			// We have an imaginary value (and we may or may not have a real part as well
			index = yValue.find_last_of("+-");

			if (index == line.npos|| index == 0)
			{
				// We seem to have an imaginary value only
				yim = atof(yValue.c_str());
				yimRead = true;
			}
			else
			{
				// We have both, real and complex values
				std::string yreValue = yValue.substr(0, index);
				std::string yimValue = yValue.substr(index);

				yre = atof(yreValue.c_str());
				yim = atof(yimValue.c_str());

				yreRead = true;
				yimRead = true;
			}
		}
		else
		{
			// We have a real value only
			yre = atof(yValue.c_str());
			yreRead = true;
		}
	}
}

void Result1DData::readLine(std::stringstream& dataContent, std::string& line)
{
	std::getline(dataContent, line);

	if (!line.empty())
	{
		if (line[line.size() - 1] == 0)
		{
			line.pop_back();
		}
	}

	if (!line.empty())
	{
		if (line[line.size() - 1] == '\r')
		{
			line.pop_back();
		}
	}
}

int Result1DData::getNumberOfQuantities()
{
	int count = 0;

	if (!yreValues.empty()) count++;
	if (!yimValues.empty()) count++;

	return count;
}


