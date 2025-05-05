//! @file CartesianPlot.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotAxis.h"
#include "OTWidgets/CartesianPlotGrid.h"
#include "OTWidgets/CartesianPlotPicker.h"
#include "OTWidgets/CartesianPlotLegend.h"
#include "OTWidgets/CartesianPlotPanner.h"
#include "OTWidgets/CartesianPlotZoomer.h"
#include "OTWidgets/CartesianPlotMagnifier.h"
#include "OTWidgets/CartesianPlotWidgetCanvas.h"

// Qwt header
#include <qwt_text.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>

// Qt header
#include <QtCore/qmath.h>
#include <QtGui/qevent.h>

ot::CartesianPlot::CartesianPlot(PlotBase* _owner)
	: AbstractPlot(_owner), m_legend(nullptr)
{
	setPlotAxis(new CartesianPlotAxis(AbstractPlotAxis::xBottom, this), nullptr, new CartesianPlotAxis(AbstractPlotAxis::yLeft, this), nullptr);

	m_canvas = new CartesianPlotWidgetCanvas(this);
	setCanvas(m_canvas);

	m_plotPanner = new CartesianPlotPanner(this);
	m_plotPanner->setMouseButton(Qt::MouseButton::MiddleButton);

	m_plotZoomer = new CartesianPlotZoomer(this);

	m_plotMagnifier = new CartesianPlotMagnifier(this);

	m_grid = new CartesianPlotGrid();
	m_grid->attach(this);

	m_picker = new CartesianPlotPicker(this);
}

ot::CartesianPlot::~CartesianPlot() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ot::CartesianPlot::updateLegend(void) {
	if (this->getConfiguration().getLegendVisible()) {
		if (!m_legend) {
			m_legend = new CartesianPlotLegend(this);
			this->insertLegend(m_legend);
		}
	}
	else {
		if (m_legend) {
			// This destroys the legend
			this->insertLegend(nullptr);
			m_legend = nullptr;
		}
	}
}

void ot::CartesianPlot::updateWholePlot(void) {
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();

	this->replot();

	m_plotZoomer->setZoomBase(false);
}

void ot::CartesianPlot::clearPlot(void) {

}

void ot::CartesianPlot::setZoomerPen(const QPen & _pen) {
	m_plotZoomer->setRubberBandPen(_pen);
	m_plotZoomer->setTrackerPen(_pen);
}

void ot::CartesianPlot::resetPlotView(void) {
	double xMin = std::numeric_limits<double>::max();
	double xMax = std::numeric_limits<double>::lowest();
	double yMin = std::numeric_limits<double>::max();
	double yMax = std::numeric_limits<double>::lowest();

	for (QwtPlotItem* item : this->itemList()) {
		if (item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
			OTAssertNullptr(dynamic_cast<QwtPlotCurve*>(item));
			QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);

			if (curve && curve->isVisible()) {
				const QwtSeriesData<QPointF>* data = curve->data();
				for (size_t i = 0; i < data->size(); ++i) {
					QPointF p = data->sample(i);
					xMin = std::min(xMin, p.x());
					xMax = std::max(xMax, p.x());
					yMin = std::min(yMin, p.y());
					yMax = std::max(yMax, p.y());
				}
			}
		}
	}

	const Plot1DCfg& cfg = this->getConfiguration();

	// If auto scale is disabled use the specified min max values for the axis
	if (!cfg.getXAxisIsAutoScale()) {
		xMin = cfg.getXAxisMin();
		xMax = cfg.getXAxisMax();
	}
	if (!cfg.getYAxisIsAutoScale()) {
		yMin = cfg.getYAxisMin();
		yMax = cfg.getYAxisMax();
	}

	// Apply scale if valid
	if (xMin < xMax && yMin < yMax) {
		this->setAxisScale(QwtPlot::xBottom, std::max(xMin, cfg.getXAxisMin()), std::min(xMax, cfg.getXAxisMax()));
		this->setAxisScale(QwtPlot::yLeft, std::max(yMin, cfg.getYAxisMin()), std::min(yMax, cfg.getYAxisMax()));
		this->updateWholePlot();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::CartesianPlot::updateGrid(void) {
	if (this->getConfiguration().getGridVisible()) {
		m_grid->setPen(QtFactory::toQColor(this->getConfiguration().getGridColor()), this->getConfiguration().getGridLineWidth(), Qt::SolidLine);
	}
	else {
		m_grid->setPen(QColor(), 0.0, Qt::NoPen);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

QwtPlotCurve * ot::CartesianPlot::findNearestCurve(const QPoint & _pos, int& _pointIx) {
	double dist{ std::numeric_limits<double>::max() };

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
				_pointIx = idx;
			}
		}
	}

	return curve;
}
