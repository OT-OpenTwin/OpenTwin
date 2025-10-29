// @otlicense

//! @file PlotDatasetData.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PlotDatasetData.h"

ot::PlotDatasetData::PlotDatasetData(std::vector<double>&& _dataX, ComplexNumberContainer* _complexNumberContainer)
	: m_dataX(std::move(_dataX)), m_dataY(_complexNumberContainer)
{
	initiate();
	_complexNumberContainer = nullptr;
}

ot::PlotDatasetData::PlotDatasetData(std::vector<double>& _dataX, ComplexNumberContainer* _complexNumberContainer)
	: m_dataX(_dataX), m_dataY(_complexNumberContainer)
{
	initiate();
	_complexNumberContainer = nullptr;
}

ot::PlotDatasetData::~PlotDatasetData()
{
	if (m_dataY != nullptr)
	{
		delete m_dataY;
		m_dataY = nullptr;
	}
}

ot::PlotDatasetData::PlotDatasetData(PlotDatasetData&& _other) noexcept
	:m_dataX(std::move(_other.m_dataX)), m_dataY(_other.m_dataY), m_numberOfDatapoints(_other.m_numberOfDatapoints)
{
	_other.m_dataY = nullptr;
}

void ot::PlotDatasetData::overrideDataY(ComplexNumberContainer* _complexNumberContainer)
{
	delete m_dataY;
	m_dataY = _complexNumberContainer;
	_complexNumberContainer = nullptr;
}

void ot::PlotDatasetData::initiate()
{
	if (m_dataX.size() == 0)
	{
		throw std::exception("Plot data set without x- axis values is not allowed.");
	}
	
	ComplexNumberContainerCartesian* cartesianData = dynamic_cast<ComplexNumberContainerCartesian*>(m_dataY);
	if (cartesianData != nullptr)
	{
		if (cartesianData->m_real.size() != 0 && m_dataX.size() != cartesianData->m_real.size() ||
			cartesianData->m_imag.size() != 0 && m_dataX.size() != cartesianData->m_imag.size())
		{
			throw std::exception("Cannot create plot with different numbers of x and y values.");
		}
		if (cartesianData->m_real.size() == 0 && cartesianData->m_imag.size() == 0)
		{
			throw std::exception("Plot data set requires data for the y axis set");
		}
	}
	else
	{
		ComplexNumberContainerPolar* polarData = dynamic_cast<ComplexNumberContainerPolar*>(m_dataY);
		assert(polarData != nullptr);
		if (polarData->m_magnitudes.size() != 0 && m_dataX.size() != polarData->m_magnitudes.size() ||
			polarData->m_phases.size() != 0 && m_dataX.size() != polarData->m_phases.size())
		{
			throw std::exception("Cannot create plot with different numbers of x and y values.");
		}
		if (polarData->m_magnitudes.size() == 0 && polarData->m_phases.size() == 0)
		{
			throw std::exception("Plot data set requires data for the y axis set");
		}
	}
	m_numberOfDatapoints = m_dataX.size();
}

