#include "stdafx.h"

/*

#include <qwt_polar_curve.h>

#include "PolarPlot.h"

// Qt header
#include <qwidget.h>
#include <qlayout.h>
#include <qevent.h>

polarPlot::polarPlot()
{
	m_centralWidget = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralWidget);

	// Create plot
	m_plot = new PolarPlotWidget;
	m_centralLayout->addWidget(m_plot);

	setWidgetStyle("color: white; background-color: #303030;", QColor(70, 70, 70), QColor(Qt::white), 0.5);

	addTestData();
}

polarPlot::~polarPlot() {
	
}

void polarPlot::setWidgetStyle(const QString & _centralWidgetStylesheet, const QColor & _plotBackground,
	const QColor & _gridColor, double _gridLineWidth) {
	m_centralWidget->setStyleSheet(_centralWidgetStylesheet);
	m_plot->setPlotBackground(QBrush(_plotBackground));
	m_plot->getGrid()->setPen(QPen(_gridColor, _gridLineWidth));
}

PolarPlotDataset * polarPlot::addDataset(double * _azimuth, double * _radius, size_t _dataSize) {
	return m_plot->addDataset(_azimuth, _radius, _dataSize);
}

void polarPlot::addTestData(void) {
	double * a = new double[200];
	double * r = new double[200];

	for (int i = 0; i < 200; i++) {
		a[i] = 360 / 200 * 4 * i;
		r[i] = 360 / 200 * i;
	}

	addDataset(a, r, 200);
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotWidget::PolarPlotWidget()
	: m_currentDataID(-1)
{
	m_canvas = canvas();
	
	m_grid = new PolarPlotGrid(this);
	m_legend = new PolarPlotLegend(this);
	m_panner = new PolarPlotPanner(m_canvas);
	m_panner = nullptr;
	m_magnifier = new PolarPlotMagnifier(this, m_canvas);
}

PolarPlotWidget::~PolarPlotWidget() {

}

QwtPolarCurve * PolarPlotWidget::findNearestCurve(QPoint _pos) {
	QwtPolarCurve * curve = nullptr;
	double dist = DBL_MAX;

	for (auto itm : m_data) {
		
	}
	return nullptr;
}

PolarPlotDataset * PolarPlotWidget::addDataset(double * _azimuth, double * _radius, size_t _dataSize) {
	PolarPlotDataset * nData = new PolarPlotDataset(++m_currentDataID, this, _azimuth, _radius, _dataSize);
	m_data.insert_or_assign(m_currentDataID, nData);
	return nData;
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotGrid::PolarPlotGrid(PolarPlotWidget * _plot) 
	: m_plot(_plot)
{
	attach(m_plot);
}

PolarPlotGrid::~PolarPlotGrid() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotLegend::PolarPlotLegend(PolarPlotWidget * _plot)
	: m_plot(_plot)
{
	m_plot->insertLegend(this);
}

PolarPlotLegend::~PolarPlotLegend() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotPanner::PolarPlotPanner(QwtPolarCanvas * _canvas)
	: QwtPolarPanner(_canvas)
{
	setMouseButton(Qt::MouseButton::MiddleButton);
	setEnabled(true);
}

PolarPlotPanner::~PolarPlotPanner() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotMagnifier::PolarPlotMagnifier(PolarPlotWidget * _plot, QwtPolarCanvas * _canvas)
	: QwtPolarMagnifier(_canvas), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot)
{
	setMouseButton(Qt::MouseButton::NoButton);
	setEnabled(true);
	m_marker = new PolarPlotMarker(m_plot);
	m_plot->setVisible(false);
}

PolarPlotMagnifier::~PolarPlotMagnifier() {

}

void PolarPlotMagnifier::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPolarMagnifier::widgetMousePressEvent(_event);
}

void PolarPlotMagnifier::widgetMouseMoveEvent(QMouseEvent * _event) {

	m_mouseMoved = true;

	if (m_rightMouseIsPressed) {
		int itemId;
		/*QwtPlotCurve * curve = m_plot->findNearestCurve(_event->pos(), itemId);

		if (curve != nullptr) {
			double x, y;
			m_plot->Owner()->findDataset(curve)->dataAt(itemId, x, y);
			m_marker->setValue(x, y);
			m_marker->setLabel(QwtText((std::to_string(x) + ", " + std::to_string(y)).c_str()));
			m_marker->setVisible(true);
			m_plot->replot();
		}*/
		/*
	}
	QwtPolarMagnifier::widgetMouseMoveEvent(_event);
}

void PolarPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPolarMagnifier::widgetMouseReleaseEvent(_event);
}

void PolarPlotMagnifier::widgetWheelEvent(QWheelEvent * _wheelEvent) {
	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (wheelFactor() != 0.0)
	{
		int delta = _wheelEvent->delta() * (-1);
		double f = qPow(wheelFactor(),
			qAbs(delta / 120.0));

		if (delta > 0)
			f = 1 / f;

		rescale(f);
	}
}

void PolarPlotMagnifier::rescale(double _factor) {
	QwtPolarMagnifier::rescale(_factor);
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotMarker::PolarPlotMarker(PolarPlotWidget * _plot) {
	attach(_plot);
}

PolarPlotMarker::~PolarPlotMarker() {

}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotData::PolarPlotData(double * _azimuth, double * _radius, size_t _dataSize) 
	: m_azimuth(_azimuth), m_radius(_radius), m_size(_dataSize)
{

}

PolarPlotData::~PolarPlotData() {

}

QwtPointPolar PolarPlotData::sample(size_t _i) const {
	if (_i < m_size) { return QwtPointPolar(m_azimuth[_i], m_radius[_i]); }
	else { assert(0); return QwtPointPolar(0, 0); }
}

QRectF PolarPlotData::boundingRect(void) const {
	if (d_boundingRect.width() < 0.0) { d_boundingRect = qwtBoundingRect(*this); }

	return d_boundingRect;
}

// ###############################################################################

// ###############################################################################

// ###############################################################################

PolarPlotDataset::PolarPlotDataset(int _id, PolarPlotWidget * _plot, double * _azimuth, double * _radius, size_t _dataSize)
	: m_plot(_plot), m_curveColor(255, 0, 0), m_curveVisible(true), m_curveWidth(1), m_isDimmed(false), m_id(_id)
{
	m_data = new PolarPlotData(_azimuth, _radius, _dataSize);

	m_curve = new QwtPolarCurve;
	m_curve->setData(m_data);
	m_curve->attach(m_plot);

	refreshVisualizationSettings();
}

PolarPlotDataset::~PolarPlotDataset() {

}

void PolarPlotDataset::setStyle(const QColor & _lineColor, double _lineWidth, bool _repaint) {
	m_curveColor = _lineColor;
	m_curveWidth = _lineWidth;
	if (_repaint) { refreshVisualizationSettings(); }
}

void PolarPlotDataset::setCurveVisible(bool _isVisible, bool _repaint) {
	m_curveVisible = _isVisible;
	if (_repaint) { refreshVisualizationSettings(); }
}

void PolarPlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_isDimmed = _isDimmed;
	if (_repaint) { refreshVisualizationSettings(); }
}

void PolarPlotDataset::findNearsetPoint(const QPoint & _pos, double & _azimuth, double & _radius, double & _dist) {
	_dist = DBL_MAX;

	for (size_t i = 0; i < m_data->size(); i++) {
		
	}

}

void PolarPlotDataset::refreshVisualizationSettings(void) {
	if (m_curveVisible) {
		if (m_isDimmed) {
			m_curve->setPen(QPen(QBrush(QColor(100, 100, 100)), 0.5));
		}
		else { m_curve->setPen(QPen(QBrush(m_curveColor), m_curveWidth)); }
	}
	else {
		m_curve->setPen(QPen(QBrush(m_curveColor), m_curveWidth, Qt::NoPen));
	}
}

*/