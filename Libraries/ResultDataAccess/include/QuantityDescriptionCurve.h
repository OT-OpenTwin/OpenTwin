#pragma once
#include "QuantityDescription.h"

class __declspec(dllexport) QuantityDescriptionCurve : public QuantityDescription
{
public:
	QuantityDescriptionCurve()
	{
		m_metadataQuantity.dataDimensions.push_back(1);
	}

	void setDataPoints(std::list<ot::Variable> _dataPoints)
	{
		m_dataPoints = std::vector<ot::Variable>{ _dataPoints.begin(), _dataPoints.end() };
	}
	const std::vector<ot::Variable>& getDataPoints() const
	{
		return m_dataPoints;
	}

private:
	std::vector<ot::Variable> m_dataPoints;
};
