// @otlicense
// File: PolarPlot.cpp
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
#include "OTCore/Math.h"
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/Polar/PolarPlot.h"
#include "OTWidgets/Plot/Polar/PolarPlotGrid.h"
#include "OTWidgets/Plot/Polar/PolarPlotAxis.h"
#include "OTWidgets/Plot/Polar/PolarPlotPanner.h"
#include "OTWidgets/Plot/Polar/PolarPlotMagnifier.h"
#include "OTWidgets/Style/GlobalColorStyle.h"
#include "OTWidgets/Widgets/Label.h"

// Qwt header
#include <qwt_polar_canvas.h>

// Qt header
#include <QtCore/qstringbuilder.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>

ot::PolarPlot::PolarPlot(PlotBase* _owner, QWidget* _parent)
	: QwtPolarPlot(_parent), AbstractPlot(_owner)
{
	m_grid = new PolarPlotGrid(this);
	m_magnifier = new PolarPlotMagnifier(this);
	m_panner = new PolarPlotPanner(this);
		
	this->setPlotAxis(new PolarPlotAxis(AbstractPlotAxis::xBottom, this), nullptr, new PolarPlotAxis(AbstractPlotAxis::yLeft, this), nullptr);
	
	slotColorStyleChanged();
	connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PolarPlot::slotColorStyleChanged);
}

ot::PolarPlot::~PolarPlot()
{
	disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PolarPlot::slotColorStyleChanged);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ot::PolarPlot::updateWholePlot() {
	updateAllAxes();

	this->replot();
}

void ot::PolarPlot::clearPlot() {

}

void ot::PolarPlot::resetPlotView() {

}

void ot::PolarPlot::updateAllAxes()
{
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::PolarPlot::updateGrid() {
	const Plot1DCfg& cfg = this->getConfig();

	QPen gridPen(QColor(), 0., Qt::NoPen);

	if (cfg.getGridVisible()) {
		gridPen = QPen(QtFactory::toQBrush(cfg.getGridColor()), cfg.getGridLineWidth(), Qt::SolidLine);
	}
	m_grid->setPen(gridPen);
}

QwtPolarCurve* ot::PolarPlot::findNearestCurve(const QwtPointPolar& _pos, size_t& _pointIx) 
{
	double tmp = 0.;
	return findNearestCurve(_pos, _pointIx, tmp);
}

QwtPolarCurve* ot::PolarPlot::findNearestCurve(const QwtPointPolar& _pos, size_t& _pointIx, double& _distance)
{
	QPoint mPos = canvas()->transform(_pos);

	_distance = std::numeric_limits<double>::max();
	QwtPolarCurve* nearest = nullptr;

	for (QwtPolarItemIterator it = itemList().begin(); it != itemList().end(); ++it)
	{
		if ((*it)->rtti() != QwtPolarItem::Rtti_PolarCurve)
		{
			continue;
		}

		QwtPolarCurve* curve = static_cast<QwtPolarCurve*>(*it);
		const QwtSeriesData<QwtPointPolar>* series = curve->data();

		const size_t size = series->size();

		for (size_t i = 0; i < size; i++)
		{
			const QwtPointPolar p = series->sample(i);
			QPoint pt = canvas()->transform(p);

			double dist = ot::Math::euclideanDistance(mPos.x(), mPos.y(), pt.x(), pt.y());
			if (dist < 0.)
			{
				dist *= (-1.);
			}

			if (dist < _distance)
			{
				_distance = dist;
				nearest = curve;
				_pointIx = i;
			}
		}
	}

	return nearest;
}

void ot::PolarPlot::keyPressEvent(QKeyEvent* _event) {
	QwtPolarPlot::keyPressEvent(_event);
	if (_event->key() == Qt::Key_Space) {
		this->resetPlotView();
	}
}

void ot::PolarPlot::mouseMoveEvent(QMouseEvent* _event) {
	OTAssertNullptr(getOwner());
	QwtPolarPlot::mouseMoveEvent(_event);
	
	QRectF pRect = plotRect();
	const int radius = pRect.width() / 2;

	const QPoint canvasPos = canvas()->mapFrom(this, _event->pos());
	QwtPointPolar polarPt = canvas()->invTransform(canvasPos);

	if (polarPt.radius() <= radius)
	{
		getOwner()->setInfoTextFromPosition(polarPt);
	}
	else
	{
		getOwner()->clearPositionInfoText();
	}
}

void ot::PolarPlot::mouseDoubleClickEvent(QMouseEvent* _event)
{
	QwtPolarPlot::mouseDoubleClickEvent(_event);
	if (_event->button() == Qt::LeftButton) {
		size_t pointIx = 0;
		PlotBase* owner = getOwner();
		OTAssertNullptr(owner);
		
		const double pixelThreshold = 150.0;

		double dist = 20000.;
		QwtPolarCurve* curve = findNearestCurve(canvas()->invTransform(_event->pos()), pointIx, dist);
		if (curve && dist < pixelThreshold) {
			PlotDataset* dataset = owner->findDataset(curve);
			if (!dataset) {
				OT_LOG_E("Failed to find dataset from curve");
				owner->requestResetItemSelection();
				return;
			}
			owner->requestCurveDoubleClicked(dataset, _event->modifiers().testFlag(Qt::ControlModifier));
		}
		else {
			owner->requestResetItemSelection();
		}
	}
}

void ot::PolarPlot::leaveEvent(QEvent* _event) {
	OTAssertNullptr(getOwner());
	QwtPolarPlot::leaveEvent(_event);

	getOwner()->clearPositionInfoText();
}

void ot::PolarPlot::slotColorStyleChanged() {
	const auto& cs = GlobalColorStyle::instance().getCurrentStyle();
	const auto& backVal = cs.getValue(ColorStyleValueEntry::PlotBackground, ColorStyleValue());
	this->setPlotBackground(backVal.toBrush());

	const auto& axisVal = cs.getValue(ColorStyleValueEntry::PlotAxis, ColorStyleValue());
	QPen axisPen(axisVal.toBrush(), 1., Qt::SolidLine);

	m_grid->setAxisPen(QwtPolar::Axis::AxisAzimuth, axisPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisBottom, axisPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisLeft, axisPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisRight, axisPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisTop, axisPen);

	update();
}