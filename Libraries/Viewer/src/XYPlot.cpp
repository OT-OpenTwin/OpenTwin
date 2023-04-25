#include "stdafx.h"

#include "xyPlot.h"
#include "Plot.h"

#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_curve.h>

#include <qevent.h>
#include <qmath.h>

xyPlot::xyPlot(Plot * _owner)
	: AbstractPlot(Cartesian), m_owner(_owner)
{
	SetAxis(new xyPlotAxis(AbstractPlotAxis::xBottom, this), nullptr, new xyPlotAxis(AbstractPlotAxis::yLeft, this), nullptr);

	m_canvas = new xyPlotWidgetCanvas(this);
	setCanvas(m_canvas);

	m_legend = new xyPlotLegend(this);

	m_plotPanner = new xyPlotPanner(this);
	m_plotPanner->setMouseButton(Qt::MouseButton::MiddleButton);

	m_plotZoomer = new xyPlotZoomer(this);

	m_plotMagnifier = new xyPlotMagnifier(this);

	m_grid = new xyPlotGrid();
	m_grid->attach(this);

	m_picker = new xyPlotPicker(this);
}

xyPlot::~xyPlot() {

}

// ################################################################

// Plot

void xyPlot::RepaintLegend(void) {
	m_legend->setVisible(m_config.legendIsVisible);
}

void xyPlot::RefreshWholePlot(void) {
	m_axisXBottom->repaint();
	m_axisYLeft->repaint();

	replot();

	m_plotZoomer->setZoomBase();
}

void xyPlot::Clear(void) {
}

void xyPlot::setZoomerPen(const QPen & _pen) {
	m_plotZoomer->setRubberBandPen(_pen);
	m_plotZoomer->setTrackerPen(_pen);
}

// ################################################################

// Grid

void xyPlot::RepaintGrid(void) {
	if (m_config.gridIsVisible) {
		m_grid->setPen(m_config.gridColor, m_config.gridLineWidth, Qt::SolidLine);
	}
	else {
		m_grid->setPen(QColor(), 0.0, Qt::NoPen);
	}
}

// ################################################################

// Getter

QwtPlotCurve * xyPlot::findNearestCurve(const QPoint & _pos, int & _pointID) {
	double dist{ DBL_MAX };

	QwtPlotCurve * curve = nullptr;

	for (QwtPlotItemIterator it = itemList().begin();
		it != itemList().end(); ++it)
	{
		if ((*it)->rtti() == QwtPlotItem::Rtti_PlotCurve)
		{
			QwtPlotCurve *c = static_cast<QwtPlotCurve *>(*it);  // c is the curve you clicked on 

			double d;
			int idx = c->closestPoint(_pos, &d);
			if (d < dist)
			{
				curve = c;
				dist = d;
				_pointID = idx;
			}
		}
	}

	return curve;
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotAxis::xyPlotAxis(AxisID _axisID, xyPlot * _plot)
	: AbstractPlotAxis(_axisID), m_plot(_plot)
{
	m_plot->setAxisScaleEngine(m_id, new QwtLinearScaleEngine());
}

xyPlotAxis::~xyPlotAxis() {

}

void xyPlotAxis::repaint(void) {
	m_plot->setAxisTitle(xyAxisID(), m_title);

	if (m_isLogScale && !m_isLogScaleLast) {
		m_isLogScaleLast = true;
		m_plot->setAxisScaleEngine(xyAxisID(), new QwtLogScaleEngine());

	}
	else if (!m_isLogScale && m_isLogScaleLast) {
		m_isLogScaleLast = false;
		m_plot->setAxisScaleEngine(xyAxisID(), new QwtLinearScaleEngine());
	}

	m_plot->setAxisAutoScale(xyAxisID(), m_isAutoScale);

	if (!m_isAutoScale) {
		m_plot->setAxisScale(xyAxisID(), m_minValue, m_maxValue);
	}

}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotWidgetCanvas::xyPlotWidgetCanvas(xyPlot * _plot) 
	: m_plot(_plot)
{

}
xyPlotWidgetCanvas::~xyPlotWidgetCanvas() {

}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotMarker::xyPlotMarker(int _id)
	: m_id(_id), m_symbol(nullptr)
{
	setLineStyle(QwtPlotMarker::LineStyle::Cross);
	setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	setLinePen(QPen(QColor(200, 150, 0), 0, Qt::DashDotLine));
	m_symbol = new QwtSymbol(QwtSymbol::Diamond, QColor(Qt::yellow), QColor(Qt::green), QSize(7, 7));
	setSymbol(m_symbol);
}

xyPlotMarker::~xyPlotMarker() {
	delete m_symbol;
}

void xyPlotMarker::setStyle(const QColor & _innerColor, const QColor & _outerColor, int _size, double _outerColorSize) {
	m_symbol->setBrush(_innerColor);
	m_symbol->setPen(_outerColor, _outerColorSize);
	m_symbol->setSize(_size);
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotPanner::xyPlotPanner(xyPlot * _plot)
	: QwtPlotPanner(_plot->canvas()), m_mouseIsPressed(false)
{
	setMouseButton(Qt::MouseButton::MiddleButton);
}

bool xyPlotPanner::eventFilter(QObject * _object, QEvent * _event) {
	if (_object == nullptr || _object != parentWidget()) { return false; }

	switch (_event->type())
	{
	case QEvent::MouseButtonPress:
	{
		auto evr = static_cast<QMouseEvent *>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = true;
			widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseMove:
	{
		QMouseEvent * evr = static_cast<QMouseEvent *>(_event);
		if (m_mouseIsPressed) {
			widgetMouseMoveEvent(evr);
			widgetMouseReleaseEvent(evr);
			setMouseButton(evr->button(), evr->modifiers());
			widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent * evr = static_cast<QMouseEvent *>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = false;
			widgetMouseReleaseEvent(evr);
		}
		break;
	}
	case QEvent::KeyPress:
	{
		//widgetKeyPressEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::KeyRelease:
	{
		//widgetKeyReleaseEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::Paint:
	{
		if (isVisible())
			return true;
		break;
	}
	default:;
	}

	return false;
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotZoomer::xyPlotZoomer(xyPlot * _plot)
	: QwtPlotZoomer(_plot->canvas()), m_plot(_plot), m_mouseMoved(false), m_controlIsPressed(false)
{

}

void xyPlotZoomer::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		m_mouseMoved = false;
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
}

void xyPlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseDoubleClickEvent(_event);
	}
	int pointId;
	m_plot->Owner()->datasetSelectionChanged(m_plot->Owner()->findDataset(m_plot->findNearestCurve(_event->pos(), pointId)));
}

void xyPlotZoomer::widgetMouseMoveEvent(QMouseEvent * _event) {
	m_mouseMoved = true;
	QwtPlotZoomer::widgetMouseMoveEvent(_event);
}

void xyPlotZoomer::widgetWheelEvent(QWheelEvent * _event) {

}

void xyPlotZoomer::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseReleaseEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		if (!m_mouseMoved) {
			QwtPlotZoomer::widgetMouseReleaseEvent(_event);
		}
	}
}

void xyPlotZoomer::widgetKeyPressEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = true;
	}
}

void xyPlotZoomer::widgetKeyReleaseEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = false;
	}
}


// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotMagnifier::xyPlotMagnifier(xyPlot * _plot)
	: QwtPlotMagnifier(_plot->canvas()), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot)
{
	m_marker = new xyPlotMarker(0);
	m_marker->attach(m_plot);
	m_marker->setVisible(false);
	m_marker->setLinePen(QColor(255, 50, 50), 0.0, Qt::DashDotDotLine);

	setMouseButton(Qt::MouseButton::NoButton);
}

void xyPlotMagnifier::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPlotMagnifier::widgetMousePressEvent(_event);
}

void xyPlotMagnifier::widgetMouseMoveEvent(QMouseEvent * _event) {

	m_mouseMoved = true;

	if (m_rightMouseIsPressed) {
		int itemId;
		QwtPlotCurve * curve = m_plot->findNearestCurve(_event->pos(), itemId);

		if (curve != nullptr) {
			double x, y;
			m_plot->Owner()->findDataset(curve)->getDataAt(itemId, x, y);
			m_marker->setValue(x, y);
			m_marker->setLabel(QwtText((std::to_string(x) + ", " + std::to_string(y)).c_str()));
			m_marker->setVisible(true);
			m_plot->replot();
		}
	}
	QwtPlotMagnifier::widgetMouseMoveEvent(_event);
}

void xyPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPlotMagnifier::widgetMouseReleaseEvent(_event);
}

void xyPlotMagnifier::widgetWheelEvent(QWheelEvent * _wheelEvent) {
	m_cursorPos = _wheelEvent->pos();

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

void xyPlotMagnifier::rescale(double _factor) {
	QwtPlot* plt = plot();
	if (plt == nullptr)
		return;

	_factor = qAbs(_factor);
	if (_factor == 1.0 || _factor == 0.0) { return; }

	bool doReplot = false;

	const bool autoReplot = plt->autoReplot();
	plt->setAutoReplot(false);

	for (int axisId = 0; axisId < QwtPlot::axisCnt; axisId++)
	{
		if (isAxisEnabled(axisId))
		{
			const QwtScaleMap scaleMap = plt->canvasMap(axisId);

			double v1 = scaleMap.s1(); //v1 is the bottom value of the axis scale
			double v2 = scaleMap.s2(); //v2 is the top value of the axis scale

			if (scaleMap.transformation())
			{
				// the coordinate system of the paint device is always linear
				v1 = scaleMap.transform(v1); // scaleMap.p1()
				v2 = scaleMap.transform(v2); // scaleMap.p2()
			}

			double c = 0; //represent the position of the cursor in the axis coordinates
			if (axisId == QwtPlot::xBottom) //we only work with these two axis
			{
				c = scaleMap.invTransform(m_cursorPos.x());
				if (scaleMap.transformation())
				{
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}

			}
			if (axisId == QwtPlot::yLeft) {
				c = scaleMap.invTransform(m_cursorPos.y());
				if (scaleMap.transformation())
				{
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}
			}
			const double center = 0.5 * (v1 + v2);
			const double width_2 = 0.5 * (v2 - v1) * _factor;
			const double newCenter = c - _factor * (c - center);

			v1 = newCenter - width_2;
			v2 = newCenter + width_2;

			if (scaleMap.transformation())
			{
				v1 = scaleMap.invTransform(v1);
				v2 = scaleMap.invTransform(v2);
			}

			plt->setAxisScale(axisId, v1, v2);
			doReplot = true;
		}
	}

	plt->setAutoReplot(autoReplot);

	if (doReplot) {
		plt->replot();
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotLegend::xyPlotLegend(xyPlot * _owner)
	: m_owner(_owner)
{
	m_owner->insertLegend(this);
}

xyPlotLegend::~xyPlotLegend() {

}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotGrid::xyPlotGrid()
	: m_color(100, 100, 100), m_width(0.5)
{
	setPen(QPen(QBrush(m_color), m_width));
}

xyPlotGrid::~xyPlotGrid() {}

void xyPlotGrid::setColor(const QColor & _color, bool _repaint) {
	m_color = _color;
	if (_repaint) {
		setPen(QPen(QBrush(m_color), m_width));
	}
}

void xyPlotGrid::setWidth(double _width, bool _repaint) {
	m_width = _width;
	if (_repaint) {
		setPen(QPen(QBrush(m_color), m_width));
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotPicker::xyPlotPicker(xyPlot * _plot)
	: QwtPlotPicker(_plot->canvas()), m_plot(_plot)
{

}

xyPlotPicker::~xyPlotPicker() {

}