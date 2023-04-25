/*
 *	File:		Plot.cpp
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <qwtw/Plot.h>
#include <qwtw/XYPlot.h>
#include <qwtw/PolarPlot.h>

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qapplication.h>

#include <qwt_plot_curve.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_canvas.h>
#include <qwt_symbol.h>

const double c_pi{ 3.14159265358979323846 };

qwtw::Plot::Plot()
	: m_currentDatasetId{ 0 }, m_isError{ false }, m_currentPlot{ AbstractPlot::Cartesian }
{
	m_centralWidget = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralWidget);

	m_errorLabel = new QLabel("Error");
	m_errorLabel->setVisible(false);

	// Create plots
	m_xyPlot = new XYPlot(this);
	m_polarPlot = new PolarPlot(this);

	m_polarPlot->setParent(nullptr);
	m_polarPlot->setVisible(false);
	m_centralLayout->addWidget(m_xyPlot);

}

qwtw::Plot::~Plot() {

}


// ###########################################################################

// Setter

void qwtw::Plot::setPlotType(AbstractPlot::PlotType _type) {
	if (_type == m_currentPlot) { return; }

	m_currentPlot = _type;

	// Change the current plot widget if the plot is currently not in an error state
	if (!m_isError) {
		switch (m_currentPlot)
		{
		case AbstractPlot::Cartesian:
			m_centralLayout->removeWidget(m_polarPlot);
			m_polarPlot->setParent(nullptr);
			m_polarPlot->setVisible(false);
			m_centralLayout->addWidget(m_xyPlot);
			m_xyPlot->setVisible(true);
			break;
		case AbstractPlot::Polar:
			m_centralLayout->removeWidget(m_xyPlot);
			m_xyPlot->setParent(nullptr);
			m_xyPlot->setVisible(false);			
			m_centralLayout->addWidget(m_polarPlot);
			m_polarPlot->setVisible(true);
			break;
		default: assert(0); return;
		}
	}
}

void qwtw::Plot::setTitle(const QString& _title) {
	m_xyPlot->setTitle(_title);
	m_polarPlot->setTitle(_title);
}

void qwtw::Plot::setGridVisible(bool _isVisible, bool _repaint) {
	m_xyPlot->SetGridVisible(_isVisible, _repaint);
	m_polarPlot->SetGridVisible(_isVisible, _repaint);
}

void qwtw::Plot::setGridColor(const QColor& _color, bool _repaint) {
	m_xyPlot->SetGridColor(_color, _repaint);
	m_polarPlot->SetGridColor(_color, _repaint);
}

void qwtw::Plot::setGridLineWidth(double _width, bool _repaint) {
	m_xyPlot->SetGridLineWidth(_width, _repaint);
	m_polarPlot->SetGridLineWidth(_width, _repaint);
}

void qwtw::Plot::setLegendVisible(bool _isVisible, bool _repaint) {
	m_xyPlot->SetLegendVisible(_isVisible, _repaint);
	m_polarPlot->SetLegendVisible(_isVisible, _repaint);
}

void qwtw::Plot::setAxisTitle(AbstractPlotAxis::AxisID _axis, const QString& _title) {
	m_xyPlot->SetAxisTitle(_axis, _title);
	m_polarPlot->SetAxisTitle(_axis, _title);
}

void qwtw::Plot::setAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _autoScaleEnabled) {
	m_xyPlot->SetAxisAutoScale(_axis, _autoScaleEnabled);
	m_polarPlot->SetAxisAutoScale(_axis, _autoScaleEnabled);
}

void qwtw::Plot::setAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _logScaleEnabled) {
	m_xyPlot->SetAxisLogScale(_axis, _logScaleEnabled);
	m_polarPlot->SetAxisLogScale(_axis, _logScaleEnabled);
}

void qwtw::Plot::setAxisMinValue(AbstractPlotAxis::AxisID _axis, double _minValue) {
	m_xyPlot->SetAxisMin(_axis, _minValue);
	m_polarPlot->SetAxisMin(_axis, _minValue);
}

void qwtw::Plot::setAxisMaxValue(AbstractPlotAxis::AxisID _axis, double _maxValue) {
	m_xyPlot->SetAxisMax(_axis, _maxValue);
	m_polarPlot->SetAxisMax(_axis, _maxValue);
}

void qwtw::Plot::setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap) {
	m_xyPlot->setAxisValueLabels(_axis, _valueToLabelMap);
	m_polarPlot->setAxisValueLabels(_axis, _valueToLabelMap);
}

void qwtw::Plot::clearAxisValueLabels() {
	clearAxisValueLabels(AbstractPlotAxis::xBottom);
	clearAxisValueLabels(AbstractPlotAxis::xTop);
	clearAxisValueLabels(AbstractPlotAxis::yLeft);
	clearAxisValueLabels(AbstractPlotAxis::yRight);
}

void qwtw::Plot::clearAxisValueLabels(AbstractPlotAxis::AxisID _axis) {
	m_xyPlot->clearAxisValueLabels(_axis);
	m_polarPlot->clearAxisValueLabels(_axis);
}

void qwtw::Plot::resetView(void) {
	assert(0);
}

// ###########################################################################

// Data handling

qwtw::PlotDataset * qwtw::Plot::addDataset(
	const QString &			_title,
	double *				_dataX,
	double *				_dataY,
	long					_dataSize,
	bool					_addToCache
) {
	PlotDataset * newData = new PlotDataset(this, m_xyPlot, m_polarPlot, ++m_currentDatasetId, _title, _dataX, _dataY, _dataSize);
	if (_addToCache) m_cache.insert_or_assign(newData->id(), std::pair<unsigned long long, PlotDataset *>(newData->id(), newData));
	return newData;
}

void qwtw::Plot::datasetSelectionChanged(PlotDataset * _selectedDataset) {
	emit selectionChanged(_selectedDataset, (QApplication::keyboardModifiers() & Qt::ControlModifier));
}

void qwtw::Plot::setErrorState(bool _isError, const QString & _message) {
	
	// Get the current or new plot widget
	// The widget depends on the current polot type
	QWidget * currentPlot;
	switch (m_currentPlot)
	{
	case AbstractPlot::Cartesian: currentPlot = m_xyPlot; break;
	case AbstractPlot::Polar: currentPlot = m_polarPlot; break;
	default: assert(0); return;
	}

	if (_isError && !m_isError) {
		
		m_centralLayout->removeWidget(currentPlot);
		currentPlot->setParent(nullptr);
		m_errorLabel->setText(_message);
		m_centralLayout->addWidget(m_errorLabel);
		m_isError = true;
	}
	else if (!_isError && m_isError) {
		m_centralLayout->removeWidget(m_errorLabel);
		m_errorLabel->setParent(nullptr);
		m_centralLayout->addWidget(currentPlot);
		m_isError = false;
	}
}

void qwtw::Plot::setIncompatibleData(void) {
	clear(false, false);
	setErrorState(true, "Incompatible data");
}

void qwtw::Plot::repaintAll(void) {
	m_xyPlot->RepaintGrid();
	m_xyPlot->RepaintLegend();

	m_polarPlot->RepaintGrid();
	m_polarPlot->RepaintLegend();
}

void qwtw::Plot::refresh(void) {
	m_xyPlot->RefreshWholePlot();
	m_polarPlot->RefreshWholePlot();
}

void qwtw::Plot::clear(bool _clearCache, bool _clearAxisSettings) {
	if (_clearCache) {
		for (auto itm : m_cache) {
			itm.second.second->detach();
			delete itm.second.second;
		}
		m_cache.clear();
	}
	else {
		for (auto itm : m_cache) {
			itm.second.second->detach();
		}
	}

	if (_clearAxisSettings) {
		m_xyPlot->setTitle("");
		m_xyPlot->SetAxisTitle(AbstractPlotAxis::xBottom, "");
		m_xyPlot->SetAxisTitle(AbstractPlotAxis::yLeft, "");

		m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, false);
		m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, false);

		m_xyPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, false);
		m_xyPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, false);

		m_xyPlot->SetAxisMax(AbstractPlotAxis::xBottom, 1.0);
		m_xyPlot->SetAxisMin(AbstractPlotAxis::xBottom, 0.0);

		m_xyPlot->SetAxisMax(AbstractPlotAxis::yLeft, 1.0);
		m_xyPlot->SetAxisMin(AbstractPlotAxis::yLeft, 0.0);

		m_polarPlot->setTitle("");
		m_polarPlot->SetAxisTitle(AbstractPlotAxis::xBottom, "");
		m_polarPlot->SetAxisTitle(AbstractPlotAxis::yLeft, "");

		m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, false);
		m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, false);

		m_polarPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, false);
		m_polarPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, false);

		m_polarPlot->SetAxisMax(AbstractPlotAxis::xBottom, 1.0);
		m_polarPlot->SetAxisMin(AbstractPlotAxis::xBottom, 0.0);

		m_polarPlot->SetAxisMax(AbstractPlotAxis::yLeft, 1.0);
		m_polarPlot->SetAxisMin(AbstractPlotAxis::yLeft, 0.0);
	}

	m_xyPlot->RefreshWholePlot();
	m_polarPlot->RefreshWholePlot();

	return;
}

void qwtw::Plot::removeFromCache(unsigned long long _entityID) {
	auto itm = m_cache.find(_entityID);
	if (itm != m_cache.end()) {
		delete itm->second.second;
		m_cache.erase(_entityID);
	}
}

qwtw::PlotDataset * qwtw::Plot::findDataset(QwtPlotCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_xyCurve == _curve) { return itm.second.second; }
	}
	return nullptr;
}

qwtw::PlotDataset * qwtw::Plot::findDataset(QwtPolarCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_polarCurve == _curve) { return itm.second.second; }
	}
	return nullptr;
}

void qwtw::Plot::setAxisQuantity(PlotDataset::axisQuantity _type) {
	for (auto itm : m_cache) {
		itm.second.second->calculateData(_type);
	}
}

// ###########################################################################

// Plot settings

void qwtw::Plot::SetStyleSheets(const std::string & _plotBackgroundSheet, const std::string & _xyPlot, const std::string & _xyCanvas, const QColor & _polarPlotColor, const QPen & _zoomerPen) {
	m_centralWidget->setStyleSheet(_plotBackgroundSheet.c_str());
	m_xyPlot->setStyleSheet(_xyPlot.c_str());
	m_xyPlot->canvas()->setStyleSheet(_xyCanvas.c_str());
	m_xyPlot->setZoomerPen(_zoomerPen);

	m_polarPlot->setPlotBackground(_polarPlotColor);

	m_xyPlot->RefreshWholePlot();
	m_polarPlot->RefreshWholePlot();
}

// ########################################################################################

// ########################################################################################

// ########################################################################################

std::string qwtw::PlotDataset::plotQuantityString(axisQuantity _quantity) {
	switch (_quantity)
	{
	case PlotDataset::Magnitude: return "Magnitude";
	case PlotDataset::Phase: return "Phase";
	case PlotDataset::Real: return "Real";
	case PlotDataset::Imaginary: return "Imaginary";
	default:
		assert(0);
		return "Real";
	}
}

qwtw::PlotDataset::axisQuantity qwtw::PlotDataset::plotQuantityFromString(const std::string& _string) {
	if (_string == "Magnitude") { return PlotDataset::Magnitude; }
	else if (_string == "Phase") { return PlotDataset::Phase; }
	else if (_string == "Real") { return PlotDataset::Real; }
	else if (_string == "Imaginary") { return PlotDataset::Imaginary; }
	else {
		assert(0);
		return PlotDataset::Real;
	}
}

qwtw::PlotDataset::PlotDataset(
	Plot *					_owner,
	XYPlot *				_xyPlot,
	PolarPlot *				_polarPlot,
	int						_id,
	const QString &			_title,
	double *				_dataX,
	double *				_dataY,
	long					_dataSize
)
	: m_id(_id), m_dataSize(_dataSize), m_dataX(_dataX), m_dataY(_dataY),
	m_owner(_owner), m_xyPlot(_xyPlot), m_polarPlot(_polarPlot), m_curveTitle(_title), m_isAttatched(false),
	m_curveColor(255, 0, 0), m_curvePenSize(1.0), m_isVisible(true), m_isDimmed(false), m_curvePointInnerColor(0, 255, 0),
	m_curvePointOutterColor(0, 255, 0), m_curvePointOutterColorWidth(1.0), m_curvePointSize(5), m_dataYim(nullptr), m_dataYcalc(nullptr),
	m_polarData(nullptr), m_dataXcalc(nullptr), m_information(nullptr)
{
	m_xyCurve = new QwtPlotCurve(_title);
	m_xyCurvePointSymbol = new QwtSymbol;
	m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarCurve = new QwtPolarCurve(_title);
	m_polarCurvePointSymbol = new QwtSymbol;
	m_polarData = new PolarPlotData(m_dataX, m_dataY, _dataSize);
	m_polarCurve->setSymbol(m_polarCurvePointSymbol);
	m_polarCurve->setData(m_polarData);
}

qwtw::PlotDataset::~PlotDataset() {
	detach();
	memFree();
}

// ###########################################################################

// Setter

void qwtw::PlotDataset::replaceData(double * _dataX, double * _dataY, long _dataSize) {
	memFree();

	m_dataSize = _dataSize;
	m_dataX = _dataX;
	m_dataY = _dataY;

	m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarData->replaceData(m_dataX, m_dataY, m_dataSize);
	m_polarCurve->setData(m_polarData);
}

void qwtw::PlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint) {
	m_isVisible = true;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurveWidth(double _penSize, bool _repaint) {
	m_curvePenSize = _penSize;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurveColor(const QColor & _color, bool _repaint) {
	m_curveColor = _color;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurvePointsVisible(bool _isVisible, bool _repaint) {
	m_curvePointsVisible = _isVisible;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurvePointInnerColor(const QColor & _color, bool _repaint) {
	m_curvePointInnerColor = _color;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurvePointOuterColor(const QColor & _color, bool _repaint) {
	m_curvePointOutterColor = _color;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurvePointSize(int _size, bool _repaint) {
	m_curvePointSize = _size;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint) {
	m_curvePointOutterColorWidth = _size;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::attach(void) {
	if (m_isAttatched) { return; }
	m_isAttatched = true;

	m_xyCurve->attach(m_xyPlot);
	m_polarCurve->attach(m_polarPlot);
}

void qwtw::PlotDataset::detach(void) {
	if (!m_isAttatched) { return; }
	m_isAttatched = false;

	m_xyCurve->detach();
	m_polarCurve->detach();
}

void qwtw::PlotDataset::setCurveTitle(const QString & _title) {
	m_curveTitle = _title;
	m_xyCurve->setTitle(m_curveTitle);
	m_polarCurve->setTitle(m_curveTitle);
}

void qwtw::PlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_isDimmed = _isDimmed;
	if (_repaint) { repaint(); }
}

void qwtw::PlotDataset::calculateData(axisQuantity _complexType) {
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

	switch (_complexType)
	{
	case PlotDataset::Magnitude:
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}

		m_xyCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYcalc, m_dataSize);
		break;
	case PlotDataset::Phase:
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataYcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / c_pi * 180;
		}
		m_xyCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYcalc, m_dataSize);
		break;
	case PlotDataset::Real:
		m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataY, m_dataSize);
		break;
	case PlotDataset::Imaginary:
		m_xyCurve->setRawSamples(m_dataX, m_dataYim, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYim, m_dataSize);
		break;
	case PlotDataset::Complex:
		m_dataYcalc = new double[m_dataSize];
		m_dataXcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataXcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / c_pi * 180;			// Phase for X
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];	// Magnitude for Y
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}
		m_xyCurve->setRawSamples(m_dataXcalc, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataXcalc, m_dataYcalc, m_dataSize);
		break;
	default:
		break;
	}

	
}

void qwtw::PlotDataset::repaint(void) {

	m_xyCurve->setVisible(m_isVisible);
	m_polarCurve->setVisible(m_isVisible);

	// Setup curve
	if (m_isVisible) {
		if (m_isDimmed) {
			m_xyCurve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine);
			m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine));
		}
		else {
			m_xyCurve->setPen(m_curveColor, m_curvePenSize, Qt::SolidLine);
			m_polarCurve->setPen(QPen(m_curveColor, m_curvePenSize, Qt::SolidLine));
		}
	}
	else {
		m_xyCurve->setPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen);
		m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen));
	}

	// Setup points
	if (m_curvePointsVisible) {

		if (m_isDimmed) {
			m_xyCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_xyCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));

			m_polarCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));
		}
		else {
			m_xyCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_xyCurvePointSymbol->setBrush(m_curvePointInnerColor);

			m_polarCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(m_curvePointInnerColor);
		}

		m_xyCurvePointSymbol->setSize(m_curvePointSize);
		m_polarCurvePointSymbol->setSize(m_curvePointSize);

		m_xyCurve->setSymbol(m_xyCurvePointSymbol);
	}
	else {
		m_xyCurve->setSymbol(nullptr);
	}
}

// ###########################################################################

// Getter

bool qwtw::PlotDataset::getDataAt(int _index, double & _x, double & _y) {
	if (_index < 0 || _index >= m_dataSize) {
		assert(0);
		return false;
	}
	_x = m_dataX[_index];
	_y = m_dataY[_index];
	return true;
}

bool qwtw::PlotDataset::getData(double *& _x, double *& _y, long & _size) {
	_x = m_dataX;
	_y = m_dataY;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool qwtw::PlotDataset::getYim(double *& _yim, long & _size) {
	_yim = m_dataYim;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool qwtw::PlotDataset::getCopyOfData(double *& _x, double *& _y, long & _size) {
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

bool qwtw::PlotDataset::getCopyOfYim(double *& _yim, long & _size) {
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

// ###########################################################################

// Private functions

void qwtw::PlotDataset::memFree(void) {
	if (m_dataX) delete[] m_dataX;
	m_dataX = nullptr;

	if (m_dataY) delete[] m_dataY;
	m_dataY = nullptr;

	if (m_dataXcalc) delete[] m_dataXcalc;
	m_dataXcalc = nullptr;
	
	if (m_dataYcalc) delete[] m_dataYcalc; 
	m_dataYcalc = nullptr;
	
	if (m_information) delete m_information;
	m_information = nullptr;
}

// ########################################################################################

// ########################################################################################

// ########################################################################################

qwtw::PolarPlotData::PolarPlotData(double * _azimuth, double * _radius, size_t _dataSize)
	: m_azimuth(_azimuth), m_radius(_radius), m_size(_dataSize)
{

}

qwtw::PolarPlotData::~PolarPlotData() {

}

QwtPointPolar qwtw::PolarPlotData::sample(size_t _i) const {
	if (_i < m_size) { return QwtPointPolar(m_azimuth[_i], m_radius[_i]); }
	else { assert(0); return QwtPointPolar(0, 0); }
}

QRectF qwtw::PolarPlotData::boundingRect(void) const {
	if (d_boundingRect.width() < 0.0) { d_boundingRect = qwtBoundingRect(*this); }

	return d_boundingRect;
}

void qwtw::PolarPlotData::replaceData(double * _azimuth, double * _radius, size_t _dataSize) {
	m_azimuth = _azimuth;
	m_radius = _radius;
	m_size = _dataSize;
}