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

void ot::PlotDatasetData::setXQuantity(Plot1DAxisCfg::AxisQuantity _quantity) {
	if (m_xQuantity == _quantity || !m_canConvert) {
		return;
	}
	else if (applyQuantity(_quantity, m_calcX)) {
		m_xQuantity = _quantity;
	}
}

void ot::PlotDatasetData::setYQuantity(Plot1DAxisCfg::AxisQuantity _quantity) {
	if (m_yQuantity == _quantity || !m_canConvert) {
		return;
	}
	else if (applyQuantity(_quantity, m_calcY)) {
		m_yQuantity = _quantity;
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

bool ot::PlotDatasetData::applyQuantity(Plot1DAxisCfg::AxisQuantity _quantity, std::vector<double>& _dataTarget) {
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

		_dataTarget = m_dataX;
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
			_dataTarget.push_back(std::abs(val));
		}
		break;

	case Plot1DAxisCfg::Phase:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(std::arg(val));
		}
		break;

	case Plot1DAxisCfg::Real:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(val.real());
		}
		break;

	case Plot1DAxisCfg::Imaginary:
		_dataTarget.reserve(m_dataY.size());
		for (const auto& val : m_dataY) {
			_dataTarget.push_back(val.imag());
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
