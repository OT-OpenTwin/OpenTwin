// @otlicense
// File: PlotDatasetData.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"
#include "OTWidgets/PlotDatasetData.h"
#include "OTWidgets/PolarPlotDatasetData.h"
#include "OTWidgets/CartesianPlotDatasetData.h"

ot::PlotDatasetData::PlotDatasetData() 
	: m_xQuantity(Plot1DAxisCfg::Undefined), m_yQuantity(Plot1DAxisCfg::Undefined), m_canConvert(false),
	m_cartesianAccessor(nullptr), m_polarAccessor(nullptr)
{}

ot::PlotDatasetData::PlotDatasetData(const std::vector<double>& _dataX, std::vector<double>&& _dataY)
	: PlotDatasetData(std::move(std::vector<double>(_dataX)), std::move(_dataY))
{}

ot::PlotDatasetData::PlotDatasetData(std::vector<double>&& _dataX, std::vector<double>&& _dataY)
	: m_xQuantity(Plot1DAxisCfg::Undefined), m_yQuantity(Plot1DAxisCfg::Undefined),
	m_calcX(std::move(_dataX)), m_calcY(std::move(_dataY)), m_canConvert(false),
	m_cartesianAccessor(nullptr), m_polarAccessor(nullptr)
{
	OTAssert(m_calcX.size() == m_calcY.size(), "Dataset size mismatch");
}

ot::PlotDatasetData::PlotDatasetData(const std::vector<double>& _dataX, std::vector<std::complex<double>>&& _dataY, Plot1DAxisCfg::AxisQuantity _initialXQuantity, Plot1DAxisCfg::AxisQuantity _initialYQuantity)
	: PlotDatasetData(std::move(std::vector<double>(_dataX)), std::move(_dataY), _initialXQuantity, _initialYQuantity)
{}

ot::PlotDatasetData::PlotDatasetData(std::vector<double>&& _dataX, std::vector<std::complex<double>>&& _dataY, Plot1DAxisCfg::AxisQuantity _initialXQuantity, Plot1DAxisCfg::AxisQuantity _initialYQuantity)
	: m_xQuantity(Plot1DAxisCfg::Undefined), m_yQuantity(Plot1DAxisCfg::Undefined),
	m_dataX(std::move(_dataX)), m_dataY(std::move(_dataY)), m_canConvert(true),
	m_cartesianAccessor(nullptr), m_polarAccessor(nullptr)
{
	if (_initialXQuantity == Plot1DAxisCfg::Undefined) {
		OT_LOG_EAS("Undefined initial X quantity (" + std::to_string(static_cast<int>(_initialXQuantity)) + "). Defaulting to XData");
		_initialXQuantity = Plot1DAxisCfg::XData;
	}
	if (_initialYQuantity == Plot1DAxisCfg::Undefined) {
		OT_LOG_EAS("Undefined initial Y quantity (" + std::to_string(static_cast<int>(_initialYQuantity)) + "). Defaulting to Real");
		_initialYQuantity = Plot1DAxisCfg::Real;
	}

	OTAssert(m_dataX.size() == m_dataY.size(), "Dataset size mismatch");

	setXQuantity(_initialXQuantity);
	setYQuantity(_initialYQuantity);

	updateData();
}

ot::PlotDatasetData::~PlotDatasetData()
{
	if (m_cartesianAccessor) {
		m_cartesianAccessor->forgetData();
		delete m_cartesianAccessor;
		m_cartesianAccessor = nullptr;
	}
	if (m_polarAccessor) {
		m_polarAccessor->forgetData();
		delete m_polarAccessor;
		m_polarAccessor = nullptr;
	}
}

void ot::PlotDatasetData::updateData()
{
	applyQuantityWithScaling(m_xQuantity, m_xQuantityScaling, m_calcX);
	applyQuantityWithScaling(m_yQuantity, m_yQuantityScaling, m_calcY);
}

void ot::PlotDatasetData::setXQuantity(Plot1DAxisCfg::AxisQuantity _quantity, bool _updateData) {
	if (m_xQuantity == _quantity || !m_canConvert) {
		return;
	}
	m_xQuantity = _quantity;
	
	if (_updateData)
	{
		applyQuantityWithScaling(m_xQuantity, m_xQuantityScaling, m_calcX);
	}
}

void ot::PlotDatasetData::setXQuantityScaling(const Plot1DAxisCfg::QuantityScaling& _scaling, bool _updateData)
{
	if (m_xQuantityScaling == _scaling || !m_canConvert) {
		return;
	}
	m_xQuantityScaling = _scaling;
	if (_updateData)
	{
		applyQuantityWithScaling(m_xQuantity, m_xQuantityScaling, m_calcX);
	}
}

void ot::PlotDatasetData::setYQuantity(Plot1DAxisCfg::AxisQuantity _quantity, bool _updateData) {
	if (m_yQuantity == _quantity || !m_canConvert) {
		return;
	}
	m_yQuantity = _quantity;
	if (_updateData)
	{
		applyQuantityWithScaling(m_yQuantity, m_yQuantityScaling, m_calcY);
	}
}

void ot::PlotDatasetData::setYQuantityScaling(const Plot1DAxisCfg::QuantityScaling& _scaling, bool _updateData)
{
	if (m_yQuantityScaling == _scaling || !m_canConvert) {
		return;
	}
	m_yQuantityScaling = _scaling;
	if (_updateData)
	{
		applyQuantityWithScaling(m_yQuantity, m_yQuantityScaling, m_calcY);
	}
}

ot::CartesianPlotDatasetData* ot::PlotDatasetData::getCartesianAccessor() {
	if (!m_cartesianAccessor) {
		m_cartesianAccessor = new CartesianPlotDatasetData(this);
	}
	return m_cartesianAccessor;
}

ot::PolarPlotDatasetData* ot::PlotDatasetData::getPolarAccessor() {
	if (!m_polarAccessor) {
		m_polarAccessor = new PolarPlotDatasetData(this);
	}
	return m_polarAccessor;
}

double ot::PlotDatasetData::scaledValue(double _value, const Plot1DAxisCfg::QuantityScaling& _scaling) const
{
	double result = _value;
	if (_scaling.has(Plot1DAxisCfg::DB10)) {
		if (result > 0.0) {
			result = Math::scaleWithDB10(_value);
		}
		else {
			result = (-200.);
		}
	}
	if (_scaling.has(Plot1DAxisCfg::DB20))
	{
		if (result > 0.0)
		{
			result = Math::scaleWithDB20(_value);
		}
		else
		{
			result = (-200.);
		}
	}
	return result;
}

bool ot::PlotDatasetData::applyQuantityWithScaling(Plot1DAxisCfg::AxisQuantity _quantity, const Plot1DAxisCfg::QuantityScaling& _scaling, std::vector<double>& _dataTarget) {
	_dataTarget.clear();
	
	if (m_polarAccessor) {
		m_polarAccessor->resetCachedRect();
	}
	if (m_cartesianAccessor) {
		m_cartesianAccessor->resetCachedRect();
	}

	if (_quantity == Plot1DAxisCfg::XData) {
		if (m_dataX.empty()) {
			return false;
		}

		_dataTarget.reserve(m_dataX.size());
		for (const auto& val : m_dataX) {
			_dataTarget.push_back(scaledValue(val, _scaling));
		}
		return true;
	}

	if (m_dataY.empty()) {
		return false;
	}

	switch (_quantity) {
	case Plot1DAxisCfg::XData:
		OT_LOG_EA("Invalid data state. This should not happen!");
		break;

	case Plot1DAxisCfg::Magnitude:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(scaledValue(std::abs(val), _scaling));
		}
		break;

	case Plot1DAxisCfg::Phase:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(scaledValue(std::arg(val), _scaling));
		}
		break;

	case Plot1DAxisCfg::Real:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(scaledValue(val.real(), _scaling));
		}
		break;

	case Plot1DAxisCfg::Imaginary:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(scaledValue(val.imag(), _scaling));
		}
		break;

	case Plot1DAxisCfg::Undefined:
		OT_LOG_E("Undefined quantity. No values applied.");
		return false;

	default:
		OT_LOG_E("Unknown quantity (" + std::to_string(static_cast<int>(_quantity)) + "). No values applied.");
		return false;
	}

	return true;
}
