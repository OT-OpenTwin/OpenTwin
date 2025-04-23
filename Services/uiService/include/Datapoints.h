#pragma once
#include <vector>

struct Datapoints
{
	std::vector<double> m_xData;
	std::vector<double> m_yData;
	void reserve(size_t numberOfDataPoints)
	{
		m_xData.reserve(numberOfDataPoints);
		m_yData.reserve(numberOfDataPoints);
	}
	void shrinkToFit()
	{
		m_xData.shrink_to_fit();
		m_yData.shrink_to_fit();
	}
};
