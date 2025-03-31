//! @file PlotDatasetData.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/PolarPlotData.h"
#include "OTWidgets/PlotDatasetData.h"

// Qwt header
#include <qwt_plot_curve.h>

namespace ot {
	namespace intern {
		const double pi{ 3.14159265358979323846 };
	}
}

ot::PlotDatasetData::PlotDatasetData(double* _dataX, double* _dataXcalc, double* _dataY, double* _dataYim, double* _dataYcalc, long _dataSize) :
	m_dataX(_dataX), m_dataXcalc(_dataXcalc), m_dataSize(_dataSize),
	m_dataY(_dataY), m_dataYim(_dataYim), m_dataYcalc(_dataYcalc)
{

}

ot::PlotDatasetData::~PlotDatasetData() {
	this->memFree();
}

ot::PlotDatasetData::PlotDatasetData(PlotDatasetData&& _other) noexcept :
	m_dataSize(std::move(_other.m_dataSize)), m_dataX(std::move(_other.m_dataX)), m_dataXcalc(std::move(_other.m_dataXcalc)),
	m_dataY(std::move(_other.m_dataY)), m_dataYim(std::move(_other.m_dataYim)), m_dataYcalc(std::move(_other.m_dataYcalc))
{
	_other.m_dataSize = 0;
	_other.m_dataX = nullptr;
	_other.m_dataXcalc = nullptr;
	_other.m_dataY = nullptr;
	_other.m_dataYim = nullptr;
	_other.m_dataYcalc = nullptr;
}

ot::PlotDatasetData& ot::PlotDatasetData::operator=(PlotDatasetData&& _other) noexcept {
	if (this != &_other) {
		this->memFree();
		m_dataSize = std::move(_other.m_dataSize);
		m_dataX = std::move(_other.m_dataX);
		m_dataXcalc = std::move(_other.m_dataXcalc);
		m_dataY = std::move(_other.m_dataY);
		m_dataYim = std::move(_other.m_dataYim);
		m_dataYcalc = std::move(_other.m_dataYcalc);

		_other.m_dataSize = 0;
		_other.m_dataX = nullptr;
		_other.m_dataXcalc = nullptr;
		_other.m_dataY = nullptr;
		_other.m_dataYim = nullptr;
		_other.m_dataYcalc = nullptr;
	}

	return *this;
}

ot::PlotDatasetData ot::PlotDatasetData::createCopy(void) const {
	long dataSize = m_dataSize;
	double* dataX = nullptr;
	double* dataXcalc = nullptr;
	double* dataY = nullptr;
	double* dataYim = nullptr;
	double* dataYcalc = nullptr;

	if (dataSize > 0) {
		if (m_dataX) {
			dataX = new double[dataSize];
			for (long i = 0; i < dataSize; i++) {
				dataX[i] = m_dataX[i];
			}
		}
		if (m_dataXcalc) {
			dataXcalc = new double[dataSize];
			for (long i = 0; i < dataSize; i++) {
				dataXcalc[i] = m_dataXcalc[i];
			}
		}

		if (m_dataY) {
			dataY = new double[dataSize];
			for (long i = 0; i < dataSize; i++) {
				dataY[i] = m_dataY[i];
			}
		}
		if (m_dataYim) {
			dataYim = new double[dataSize];
			for (long i = 0; i < dataSize; i++) {
				dataYim[i] = m_dataYim[i];
			}
		}
		if (m_dataYcalc) {
			dataYcalc = new double[dataSize];
			for (long i = 0; i < dataSize; i++) {
				dataYcalc[i] = m_dataYcalc[i];
			}
		}
	}

	return PlotDatasetData(dataX, dataXcalc, dataY, dataYim, dataYcalc, dataSize);
}

void ot::PlotDatasetData::calculateData(Plot1DCfg::AxisQuantity _axisQuantity, QwtPlotCurve* _cartesianCurve, PolarPlotData* _polarData) {
	if (m_dataYcalc != nullptr) {
		delete[] m_dataYcalc;
		m_dataYcalc = nullptr;
	}
	if (m_dataXcalc != nullptr) {
		delete[] m_dataXcalc;
		m_dataXcalc = nullptr;
	}
	if (m_dataSize == 0) {
		return;
	}

	switch (_axisQuantity) {
	case Plot1DCfg::Magnitude:
		if (!m_dataYim) {
			OT_LOG_EAS("No imaginary data available for magnitude calculation");
			return;
		}

		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}

		_cartesianCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		_polarData->setData(m_dataX, m_dataYcalc, m_dataSize);
		break;

	case Plot1DCfg::Phase:
		if (!m_dataYim) {
			OT_LOG_EAS("No imaginary data available for phase calculation");
			return;
		}

		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataYcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / intern::pi * 180;
		}
		_cartesianCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		_polarData->setData(m_dataX, m_dataYcalc, m_dataSize);
		break;

	case Plot1DCfg::Real:
		_cartesianCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);
		_polarData->setData(m_dataX, m_dataY, m_dataSize);
		break;

	case Plot1DCfg::Imaginary:
		if (!m_dataYim) {
			OT_LOG_EAS("No imaginary data available for imaginary calculation");
			return;
		}

		_cartesianCurve->setRawSamples(m_dataX, m_dataYim, m_dataSize);
		_polarData->setData(m_dataX, m_dataYim, m_dataSize);
		break;

	case Plot1DCfg::Complex:
		if (!m_dataYim) {
			OT_LOG_EAS("No imaginary data available for complex calculation");
			return;
		}

		m_dataYcalc = new double[m_dataSize];
		m_dataXcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataXcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / intern::pi * 180;			// Phase for X
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];	// Magnitude for Y
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}
		_cartesianCurve->setRawSamples(m_dataXcalc, m_dataYcalc, m_dataSize);
		_polarData->setData(m_dataXcalc, m_dataYcalc, m_dataSize);
		break;

	default:
		OT_LOG_EAS("Unknown axis quantity (" + std::to_string((int)_axisQuantity) + ")");
		break;
	}
}

void ot::PlotDatasetData::replaceData(double* _dataX, double* _dataXcalc, double* _dataY, double* _dataYim, double* _dataYcalc, long _dataSize) {
	this->memFree();

	m_dataSize = _dataSize;

	m_dataX = _dataX;
	m_dataXcalc = _dataXcalc;

	m_dataY = _dataY;
	m_dataYim = _dataYim;
	m_dataYcalc = _dataYcalc;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

bool ot::PlotDatasetData::getDataAt(int _index, double& _x, double& _y) {
	if (_index < 0 || _index >= m_dataSize) {
		OT_LOG_EAS("Index (" + std::to_string(_index) + ") out of range");
		return false;
	}
	_x = m_dataX[_index];
	_y = m_dataY[_index];
	return true;
}

bool ot::PlotDatasetData::getData(double*& _x, double*& _y, long& _size) {
	_x = m_dataX;
	_y = m_dataY;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool ot::PlotDatasetData::getYim(double*& _yim, long& _size) {
	if (!m_dataYim && m_dataSize > 0) {
		OT_LOG_EAS("No imaginary data available");
	}

	_yim = m_dataYim;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool ot::PlotDatasetData::getCopyOfData(double*& _x, double*& _y, long& _size) {
	_size = m_dataSize;
	if (m_dataSize == 0) {
		return false;
	}

	_x = new double[m_dataSize];
	_y = new double[m_dataSize];

	for (int i = 0; i < m_dataSize; i++) {
		_x[i] = m_dataX[i];
		_y[i] = m_dataY[i];
	}
	return true;
}

bool ot::PlotDatasetData::getCopyOfYim(double*& _yim, long& _size) {
	_size = m_dataSize;
	if (m_dataSize == 0) {
		return false;
	}
	_yim = new double[m_dataSize];

	for (int i = 0; i < m_dataSize; i++) {
		_yim[i] = m_dataYim[i];
	}
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::PlotDatasetData::memFree(void) {
	m_dataSize = 0;
	
	if (m_dataX) {
		delete[] m_dataX;
		m_dataX = nullptr;
	}
	if (m_dataXcalc) {
		delete[] m_dataXcalc;
		m_dataXcalc = nullptr;
	}

	if (m_dataY) {
		delete[] m_dataY;
		m_dataY = nullptr;
	}
	if (m_dataYim) {
		delete[] m_dataYim;
		m_dataYim = nullptr;
	}
	if (m_dataYcalc) {
		delete[] m_dataYcalc;
		m_dataYcalc = nullptr;
	}
}
