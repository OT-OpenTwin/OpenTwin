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
#include "OTWidgets/Label.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotGrid.h"
#include "OTWidgets/PolarPlotAxis.h"
#include "OTWidgets/PolarPlotLegend.h"
#include "OTWidgets/PolarPlotPanner.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PolarPlotMagnifier.h"

// Qwt header
#include <qwt_polar_canvas.h>

// Qt header
#include <QtCore/qstringbuilder.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>

ot::PolarPlot::PolarPlot(PlotBase* _owner, QWidget* _parent)
	: QwtPolarPlot(_parent), AbstractPlot(_owner), m_legend(nullptr)
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

void ot::PolarPlot::updateLegend() {
	if (this->getConfiguration().getLegendVisible()) {
		if (!m_legend) {
			m_legend = new PolarPlotLegend(this);
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

void ot::PolarPlot::updateWholePlot() {
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();

	this->replot();
}

void ot::PolarPlot::clearPlot() {

}

void ot::PolarPlot::resetPlotView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::PolarPlot::updateGrid() {
	const Plot1DCfg& config = this->getConfiguration();

	QPen gridPen(QColor(), 0., Qt::NoPen);

	if (config.getGridVisible()) {
		gridPen = QPen(QtFactory::toQBrush(config.getGridColor()), config.getGridLineWidth(), Qt::SolidLine);
	}
	m_grid->setPen(gridPen);
}

QwtPolarCurve* ot::PolarPlot::findNearestCurve(const QwtPointPolar& _pos, size_t& _pointIx) {
	QPoint mPos = canvas()->transform(_pos);

	double minDist = std::numeric_limits<double>::max();
	QwtPolarCurve* nearest = nullptr;

	for (QwtPolarItemIterator it = itemList().begin(); it != itemList().end(); ++it) {
		if ((*it)->rtti() != QwtPolarItem::Rtti_PolarCurve) {
			continue;
		}

		QwtPolarCurve* curve = static_cast<QwtPolarCurve*>(*it);
		const QwtSeriesData<QwtPointPolar>* series = curve->data();

		const size_t size = series->size();

		for (size_t i = 0; i < size; i++) {
			const QwtPointPolar p = series->sample(i);
			QPoint pt = canvas()->transform(p);

			double dist = ot::Math::euclideanDistance(mPos.x(), mPos.y(), pt.x(), pt.y());
			if (dist < 0.) {
				dist *= (-1.);
			}

			if (dist < minDist) {
				minDist = dist;
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
	
	const QwtPointPolar polarPt = canvas()->invTransform(_event->pos());
	getOwner()->setInfoTextFromPosition(polarPt);
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