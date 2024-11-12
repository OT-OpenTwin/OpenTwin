//! @file PlotDataset.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Plot.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/PolarPlotData.h"

// Qwt header
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include <qwt_polar_curve.h>

namespace ot {
	namespace intern {
		const double pi{ 3.14159265358979323846 };
	}
}

ot::PlotDataset::PlotDataset(Plot* _ownerPlot, int _id, const QString& _title, double* _dataX, double* _dataY, long _dataSize)
	: m_id(_id), m_dataSize(_dataSize), m_dataX(_dataX), m_dataY(_dataY),
	m_ownerPlot(_ownerPlot), m_curveTitle(_title), m_isAttatched(false),
	m_curveColor(255, 0, 0), m_curvePenSize(1.0), m_isVisible(true), m_isDimmed(false), m_curvePointInnerColor(0, 255, 0),
	m_curvePointOutterColor(0, 255, 0), m_curvePointOutterColorWidth(1.0), m_curvePointSize(5), m_dataYim(nullptr), m_dataYcalc(nullptr),
	m_curvePointsVisible(false), m_curveTreeItemID(0), m_polarData(nullptr), m_dataXcalc(nullptr), 
	m_entityID(0), m_entityVersion(0), m_curveEntityID(0), m_curveEntityVersion(0)
{
	m_cartesianCurve = new QwtPlotCurve(_title);
	m_cartesianCurvePointSymbol = new QwtSymbol;
	m_cartesianCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarCurve = new QwtPolarCurve(_title);
	m_polarCurvePointSymbol = new QwtSymbol;
	m_polarData = new PolarPlotData(m_dataX, m_dataY, _dataSize);
	m_polarCurve->setSymbol(m_polarCurvePointSymbol);
	m_polarCurve->setData(m_polarData);
}

ot::PlotDataset::~PlotDataset() {
	this->detach();
	this->memFree();
}

// ###########################################################################

// Setter

void ot::PlotDataset::replaceData(double* _dataX, double* _dataY, long _dataSize) {
	this->memFree();

	m_dataSize = _dataSize;
	m_dataX = _dataX;
	m_dataY = _dataY;

	m_cartesianCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarData->setData(m_dataX, m_dataY, m_dataSize);
	m_polarCurve->setData(m_polarData);
}

void ot::PlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint) {
	m_isVisible = true;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurveWidth(double _penSize, bool _repaint) {
	m_curvePenSize = _penSize;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurveColor(const QColor& _color, bool _repaint) {
	m_curveColor = _color;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurvePointsVisible(bool _isVisible, bool _repaint) {
	m_curvePointsVisible = _isVisible;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurvePointInnerColor(const QColor& _color, bool _repaint) {
	m_curvePointInnerColor = _color;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColor(const QColor& _color, bool _repaint) {
	m_curvePointOutterColor = _color;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurvePointSize(int _size, bool _repaint) {
	m_curvePointSize = _size;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint) {
	m_curvePointOutterColorWidth = _size;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::attach(void) {
	if (m_isAttatched) {
		//OT_LOG_WA("Dataset already attached");
		return;
	}
	m_isAttatched = true;

	m_cartesianCurve->attach(m_ownerPlot->getCartesianPlot());
	m_polarCurve->attach(m_ownerPlot->getPolarPlot());
}

void ot::PlotDataset::detach(void) {
	if (!m_isAttatched) {
		return;
	}
	m_isAttatched = false;

	m_cartesianCurve->detach();
	m_polarCurve->detach();
}

void ot::PlotDataset::setCurveTitle(const QString& _title) {
	m_curveTitle = _title;
	m_cartesianCurve->setTitle(m_curveTitle);
	m_polarCurve->setTitle(m_curveTitle);
}

void ot::PlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_isDimmed = _isDimmed;
	if (_repaint) {
		this->updateVisualization();
	}
}

void ot::PlotDataset::calculateData(Plot1DCfg::AxisQuantity _axisQuantity) {
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
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}

		m_cartesianCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->setData(m_dataX, m_dataYcalc, m_dataSize);
		break;

	case Plot1DCfg::Phase:
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataYcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / intern::pi * 180;
		}
		m_cartesianCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->setData(m_dataX, m_dataYcalc, m_dataSize);
		break;

	case Plot1DCfg::Real:
		m_cartesianCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);
		m_polarData->setData(m_dataX, m_dataY, m_dataSize);
		break;

	case Plot1DCfg::Imaginary:
		m_cartesianCurve->setRawSamples(m_dataX, m_dataYim, m_dataSize);
		m_polarData->setData(m_dataX, m_dataYim, m_dataSize);
		break;

	case Plot1DCfg::Complex:
		m_dataYcalc = new double[m_dataSize];
		m_dataXcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataXcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / intern::pi * 180;			// Phase for X
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];	// Magnitude for Y
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}
		m_cartesianCurve->setRawSamples(m_dataXcalc, m_dataYcalc, m_dataSize);
		m_polarData->setData(m_dataXcalc, m_dataYcalc, m_dataSize);
		break;

	default:
		OT_LOG_EAS("Unknown axis quantity (" + std::to_string((int)_axisQuantity) + ")");
		break;
	}


}

// ###########################################################################

// Getter

bool ot::PlotDataset::getDataAt(int _index, double& _x, double& _y) {
	if (_index < 0 || _index >= m_dataSize) {
		OT_LOG_EAS("Index (" + std::to_string(_index) + ") out of range");
		return false;
	}
	_x = m_dataX[_index];
	_y = m_dataY[_index];
	return true;
}

bool ot::PlotDataset::getData(double*& _x, double*& _y, long& _size) {
	_x = m_dataX;
	_y = m_dataY;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool ot::PlotDataset::getYim(double*& _yim, long& _size) {
	_yim = m_dataYim;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool ot::PlotDataset::getCopyOfData(double*& _x, double*& _y, long& _size) {
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

bool ot::PlotDataset::getCopyOfYim(double*& _yim, long& _size) {
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

void ot::PlotDataset::updateVisualization(void) {

	m_cartesianCurve->setVisible(m_isVisible);
	m_polarCurve->setVisible(m_isVisible);

	// Setup curve
	if (m_isVisible) {
		if (m_isDimmed) {
			m_cartesianCurve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine);
			m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine));
		}
		else {
			m_cartesianCurve->setPen(m_curveColor, m_curvePenSize, Qt::SolidLine);
			m_polarCurve->setPen(QPen(m_curveColor, m_curvePenSize, Qt::SolidLine));
		}
	}
	else {
		m_cartesianCurve->setPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen);
		m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen));
	}

	// Setup points
	if (m_curvePointsVisible) {

		if (m_isDimmed) {
			m_cartesianCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_cartesianCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));

			m_polarCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));
		}
		else {
			m_cartesianCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_cartesianCurvePointSymbol->setBrush(m_curvePointInnerColor);

			m_polarCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(m_curvePointInnerColor);
		}

		m_cartesianCurvePointSymbol->setSize(m_curvePointSize);
		m_polarCurvePointSymbol->setSize(m_curvePointSize);

		m_cartesianCurve->setSymbol(m_cartesianCurvePointSymbol);
	}
	else {
		m_cartesianCurve->setSymbol(nullptr);
	}
}

// ###########################################################################

// Private functions

void ot::PlotDataset::memFree(void) {
	if (m_dataX) {
		delete[] m_dataX;
		m_dataX = nullptr;
	}
	if (m_dataY) {
		delete[] m_dataY;
		m_dataY = nullptr;
	}
	if (m_dataXcalc) {
		delete[] m_dataXcalc;
		m_dataXcalc = nullptr;
	}
	if (m_dataYcalc) {
		delete[] m_dataYcalc;
		m_dataYcalc = nullptr;
	}
	m_dataSize = 0;
}
