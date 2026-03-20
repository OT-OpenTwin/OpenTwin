// @otlicense
// File: CartesianPlot.cpp
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
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlot.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotAxis.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotGrid.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotPicker.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotPanner.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotZoomer.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotMagnifier.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotWidgetCanvas.h"

// Qwt header
#include <qwt_text.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>

// Qt header
#include <QtCore/qmath.h>
#include <QtGui/qevent.h>

ot::CartesianPlot::CartesianPlot(PlotBase* _owner, QWidget* _parent)
	: QwtPlot(_parent), AbstractPlot(_owner)
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

// Virtual methods

void ot::CartesianPlot::updateWholePlot() {
	updateAllAxes();

	this->replot();

	m_plotZoomer->setZoomBase(false);
}

void ot::CartesianPlot::clearPlot() {

}

void ot::CartesianPlot::setZoomerPen(const QPen & _pen) {
	m_plotZoomer->setRubberBandPen(_pen);
	m_plotZoomer->setTrackerPen(_pen);
}

void ot::CartesianPlot::resetPlotView() {
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

	PlotBase* base = this->getOwner();
	OTAssertNullptr(base);
	const Plot1DCfg& cfg = base->getConfig();

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

void ot::CartesianPlot::updateAllAxes()
{
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::CartesianPlot::updateGrid() {
	const Plot1DCfg& cfg = this->getConfig();
	if (cfg.getGridVisible()) {
		m_grid->setPen(QPen(QtFactory::toQBrush(cfg.getGridColor()), cfg.getGridLineWidth(), Qt::SolidLine));
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

	for (QwtPlotItemIterator it = itemList().begin(); it != itemList().end(); ++it)
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

// ###########################################################################################################################################################################################################################################################################################################################

// Event handling

void ot::CartesianPlot::keyPressEvent(QKeyEvent* _event)
{
	QwtPlot::keyPressEvent(_event);
	if (_event->key() == Qt::Key_Space)
	{
		resetPlotView();
	}
}

void ot::CartesianPlot::mouseDoubleClickEvent(QMouseEvent* _event)
{
	QwtPlot::mouseDoubleClickEvent(_event);
	if (_event->button() == Qt::LeftButton)
	{
		int ix = 0;
		QwtPlotCurve* curve = this->findNearestCurve(_event->pos(), ix);

		if (curve)
		{
			QPointF point = curve->sample(ix);

			// Point in pixel coordinates
			QPointF pxPoint(transform(xBottom, point.x()), transform(yLeft, point.y()));

			// Distance to the mouse position in pixel coordinates
			double dist = QPointF(_event->position() - pxPoint).manhattanLength();

			const double pixelThreshold = 150.0;

			if (dist <= pixelThreshold)
			{
				PlotDataset* dataset = this->getOwner()->findDataset(curve);
				if (!dataset)
				{
					OT_LOG_E("Failed to find dataset from curve");
					return;
				}

				this->getOwner()->requestCurveDoubleClicked(dataset, _event->modifiers() & Qt::KeyboardModifier::ControlModifier);
			}
			else
			{
				this->getOwner()->requestResetItemSelection();
			}
		}
		else
		{
			this->getOwner()->requestResetItemSelection();
		}
	}
}

void ot::CartesianPlot::mouseMoveEvent(QMouseEvent* _event)
{
	OTAssertNullptr(getOwner());

	QwtPlot::mouseMoveEvent(_event);
	
	const double xValue = invTransform(xBottom, _event->position().x());
	const double yValue = invTransform(yLeft, _event->position().y());

	getOwner()->setInfoTextFromPosition(QPointF(xValue, yValue));

}

void ot::CartesianPlot::leaveEvent(QEvent* _event)
{
	OTAssertNullptr(getOwner());
	QwtPlot::leaveEvent(_event);

	getOwner()->clearPositionInfoText();
}