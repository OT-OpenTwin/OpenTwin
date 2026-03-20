// @otlicense
// File: PlotDataset.cpp
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
#include "OTGui/Painter/Painter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Style/GlobalColorStyle.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotLegendItem.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlot.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotCurve.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotDatasetData.h"
#include "OTWidgets/Plot/Polar/PolarPlot.h"
#include "OTWidgets/Plot/Polar/PolarPlotCurve.h"
#include "OTWidgets/Plot/Polar/PolarPlotDatasetData.h"

// Qwt header
#include <qwt_symbol.h>

// Qt header
#include <QtCore/qthread.h>

QwtSymbol::Style ot::PlotDataset::toQwtSymbolStyle(Plot1DCurveCfg::Symbol _symbol)
{
	switch (_symbol)
	{
	case ot::Plot1DCurveCfg::NoSymbol: return QwtSymbol::Style::NoSymbol;
	case ot::Plot1DCurveCfg::Circle: return QwtSymbol::Style::Ellipse;
	case ot::Plot1DCurveCfg::Square: return QwtSymbol::Style::Rect;
	case ot::Plot1DCurveCfg::Diamond: return QwtSymbol::Style::Diamond;
	case ot::Plot1DCurveCfg::TriangleUp: return QwtSymbol::Style::Triangle;
	case ot::Plot1DCurveCfg::TriangleDown: return QwtSymbol::Style::DTriangle;
	case ot::Plot1DCurveCfg::TriangleLeft: return QwtSymbol::Style::LTriangle;
	case ot::Plot1DCurveCfg::TriangleRight: return QwtSymbol::Style::RTriangle;
	case ot::Plot1DCurveCfg::Cross: return QwtSymbol::Style::Cross;
	case ot::Plot1DCurveCfg::XCross: return QwtSymbol::Style::XCross;
	case ot::Plot1DCurveCfg::HLine: return QwtSymbol::Style::HLine;
	case ot::Plot1DCurveCfg::VLine: return QwtSymbol::Style::VLine;
	case ot::Plot1DCurveCfg::Star6: return QwtSymbol::Style::Star2;
	case ot::Plot1DCurveCfg::Star8: return QwtSymbol::Style::Star1;
	case ot::Plot1DCurveCfg::Hexagon: return QwtSymbol::Style::Hexagon;
	default:
		OT_LOG_EAS("Unknown symbol (" + std::to_string((int)_symbol) + ")");
		return QwtSymbol::Style::NoSymbol;
	}
}

ot::Plot1DCurveCfg::Symbol ot::PlotDataset::toPlot1DCurveSymbol(QwtSymbol::Style _symbol)
{
	switch (_symbol)
	{
	case QwtSymbol::NoSymbol: return Plot1DCurveCfg::NoSymbol;
	case QwtSymbol::Ellipse: return Plot1DCurveCfg::Circle;
	case QwtSymbol::Rect: return Plot1DCurveCfg::Square;
	case QwtSymbol::Diamond: return Plot1DCurveCfg::Diamond;
	case QwtSymbol::Triangle: return Plot1DCurveCfg::TriangleUp;
	case QwtSymbol::DTriangle: return Plot1DCurveCfg::TriangleDown;
	case QwtSymbol::LTriangle: return Plot1DCurveCfg::TriangleLeft;
	case QwtSymbol::RTriangle: return Plot1DCurveCfg::TriangleRight;
	case QwtSymbol::Cross: return Plot1DCurveCfg::Cross;
	case QwtSymbol::XCross: return Plot1DCurveCfg::XCross;
	case QwtSymbol::HLine: return Plot1DCurveCfg::HLine;
	case QwtSymbol::VLine: return Plot1DCurveCfg::VLine;
	case QwtSymbol::Star1: return Plot1DCurveCfg::Star8;
	case QwtSymbol::Star2: return Plot1DCurveCfg::Star6;
	case QwtSymbol::Hexagon: return Plot1DCurveCfg::Hexagon;
	default:
		OT_LOG_EAS("Unknown symbol (" + std::to_string((int)_symbol) + ")");
		return Plot1DCurveCfg::NoSymbol;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::PlotDataset::PlotDataset(PlotBase* _ownerPlot, const Plot1DCurveCfg& _config, PlotDatasetData&& _data) :
	m_config(_config), m_data(std::move(_data)), m_legendItem(nullptr),
	m_cartesianCurve(nullptr), m_cartesianCurvePointSymbol(nullptr),
	m_polarCurve(nullptr), m_polarCurvePointSymbol(nullptr)
{
	buildCartesianCurve();
	buildPolarCurve();

	if (_ownerPlot != nullptr)
	{
		setOwnerPlot(_ownerPlot);
	}
}

ot::PlotDataset::~PlotDataset()
{
	this->detach();

	if (m_polarCurve != nullptr)
	{
		delete m_polarCurve;
		m_polarCurve = nullptr;
	}
	if (m_cartesianCurve != nullptr)
	{
		delete m_cartesianCurve;
		m_cartesianCurve = nullptr;
	}

	if (m_legendItem != nullptr)
	{
		delete m_legendItem;
		m_legendItem = nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Attach / Detach

void ot::PlotDataset::attach()
{
	OTAssertNullptr(m_ownerPlot);

	if (m_isAttatched)
	{
		return;
	}
	m_isAttatched = true;

	AbstractPlot* plot = m_ownerPlot->getCurrentPlot();
	if (m_ownerPlot->getCurrentPlotType() == Plot1DCfg::PlotType::Cartesian)
	{
		CartesianPlot* cartesianPlot = dynamic_cast<CartesianPlot*>(plot);
		OTAssertNullptr(cartesianPlot);
		m_cartesianCurve->attach(cartesianPlot);
	}
	else
	{
		PolarPlot* polarPlot = dynamic_cast<PolarPlot*>(plot);
		OTAssertNullptr(polarPlot);
		m_polarCurve->attach(polarPlot);
	}

	createLegendItem();
	OTAssertNullptr(m_legendItem);
	m_legendItem->attach();
	updateLegendVisualization();
}

void ot::PlotDataset::detach()
{
	if (!m_isAttatched)
	{
		return;
	}

	m_isAttatched = false;

	if (m_cartesianCurve != nullptr)
	{
		m_cartesianCurve->detach();
	}

	if (m_polarCurve != nullptr)
	{
		m_polarCurve->detach();
	}

	if (m_legendItem)
	{
		m_legendItem->detach();
	}
}

void ot::PlotDataset::rebuildCurve(bool _reattach)
{
	OTAssertNullptr(m_ownerPlot);

	switch (m_ownerPlot->getCurrentPlotType())
	{
	case Plot1DCfg::PlotType::Cartesian:
		buildCartesianCurve();
		break;

	case Plot1DCfg::PlotType::Polar:
		buildPolarCurve();
		break;

	default:
		OT_LOG_E("Unknown plot type (" + std::to_string(static_cast<int>(m_ownerPlot->getCurrentPlotType())) + ")");
		break;
	}

	if (_reattach)
	{
		this->detach();
		this->attach();
	}
}

void ot::PlotDataset::setOwnerPlot(PlotBase* _ownerPlot)
{
	OTAssertNullptr(_ownerPlot);
	m_ownerPlot = _ownerPlot;

	if (m_ownerPlot)
	{
		createLegendItem();
		m_legendItem->setLegend(m_ownerPlot->getLegend());
	}
	else if (m_legendItem)
	{
		m_legendItem->forgetLegend();
	}

	rebuildCurve();
}

// ###########################################################################################################################################################################################################################################################################################################################

// General Setter/Getter

ot::CartesianPlotCurve* ot::PlotDataset::getCartesianCurve()
{
	if (m_cartesianCurve != nullptr)
	{
		return m_cartesianCurve;
	}
	else
	{
		buildCartesianCurve();
		attach();
		return m_cartesianCurve;
	}
}

ot::PolarPlotCurve* ot::PlotDataset::getPolarCurve()
{
	if (m_polarCurve != nullptr)
	{
		return m_polarCurve;
	}
	else
	{
		buildPolarCurve();
		attach();
		return m_polarCurve;
	}
}

void ot::PlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint)
{
	m_config.setVisible(_isVisible);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurveWidth(double _penSize, bool _repaint)
{
	m_config.setLinePenWidth(_penSize);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurveColor(const Color& _color, bool _repaint)
{
	m_config.setLinePenColor(_color);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointInnerColor(const Color& _color, bool _repaint)
{
	m_config.setPointFillColor(_color);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColor(const Color& _color, bool _repaint)
{
	m_config.setPointOutlinePenColor(_color);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointSize(int _size, bool _repaint)
{
	m_config.setPointSize(_size);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint)
{
	m_config.setPointOutlinePenWidth(_size);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setDimmed(bool _isDimmed, bool _repaint)
{
	m_config.setDimmed(_isDimmed);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setPointInterval(int _interval, bool _repaint)
{
	m_config.setPointInterval(_interval);
	if (_repaint)
	{
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setAxisQuantitiesAndScaling(Plot1DAxisCfg::AxisQuantity _xQuantity, const Plot1DAxisCfg::QuantityScaling& _xQuantityScaling, Plot1DAxisCfg::AxisQuantity _yQuantity, const Plot1DAxisCfg::QuantityScaling& _yQuantityScaling)
{
	m_data.setXQuantity(_xQuantity);
	m_data.setXQuantityScaling(_xQuantityScaling);
	m_data.setYQuantity(_yQuantity);
	m_data.setYQuantityScaling(_yQuantityScaling);
	m_data.updateData();
}

void ot::PlotDataset::setSelected(bool _isSelected)
{
	if (_isSelected == m_isSelected)
	{
		return;
	}

	m_isSelected = _isSelected;

	this->updateCurveVisualization();
}

void ot::PlotDataset::setHighlighted(bool _hasHighlight)
{
	double zVal = PlotBase::ItemZOrder::visibleCurves();

	if (!m_isSelected)
	{
		zVal = PlotBase::ItemZOrder::dimmedCurves();
	}

	if (_hasHighlight)
	{
		zVal = PlotBase::ItemZOrder::highlightedCurves();
	}

	if (m_cartesianCurve)
	{
		m_cartesianCurve->setHighlight(_hasHighlight);
		m_cartesianCurve->setZ(zVal);
	}
	if (m_polarCurve)
	{
		m_polarCurve->setHighlight(_hasHighlight);
		m_polarCurve->setZ(zVal);
	}

	OTAssertNullptr(m_ownerPlot);
	m_ownerPlot->replot();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data Setter / Getter

void ot::PlotDataset::updateCurveVisualization()
{
	PenFCfg linePenCfg(m_config.getLinePen());
	const PenFCfg& pointOutlinePenCfg = m_config.getPointOutlinePen();

	QPen linePen = QtFactory::toQPen(linePenCfg);

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	const ColorStyleValue& dimmedColorValue = cs.getValue(ColorStyleValueEntry::PlotCurveDimmed);

	QBrush dimmedBrush = dimmedColorValue.toBrush();

	QPen dimmedPen = linePen;
	dimmedPen.setBrush(dimmedBrush);
	dimmedPen.setWidthF(linePen.width());

	QPen invisPen(QBrush(), 0., Qt::NoPen);

	QPen highlightPen = linePen;
	highlightPen.setBrush(cs.getValue(ColorStyleValueEntry::PlotCurveHighlight).toBrush());
	highlightPen.setWidthF(linePen.width() + 2.);

	double zVal = PlotBase::ItemZOrder::visibleCurves();

	if (!m_isSelected)
	{
		zVal = PlotBase::ItemZOrder::dimmedCurves();
	}

	if (m_cartesianCurve)
	{
		m_cartesianCurve->setHighlightPen(highlightPen);
		m_cartesianCurve->setZ(zVal);
	}
	if (m_polarCurve)
	{
		m_polarCurve->setHighlightPen(highlightPen);
		m_polarCurve->setZ(zVal);
	}

	// Setup curve pen
	if (m_config.getVisible())
	{
		if (m_config.getDimmed())
		{
			// Dimmend curve pen
			if (m_cartesianCurve)
			{
				m_cartesianCurve->setPen(dimmedPen);
			}
			if (m_polarCurve)
			{
				m_polarCurve->setPen(dimmedPen);
			}
		}
		else
		{
			// Regular curve pen
			if (m_cartesianCurve)
			{
				m_cartesianCurve->setPen(linePen);
			}
			if (m_polarCurve)
			{
				m_polarCurve->setPen(linePen);
			}
		}
	}
	else
	{
		// Invisible curve pen
		if (m_cartesianCurve)
		{
			m_cartesianCurve->setPen(invisPen);
		}
		if (m_polarCurve)
		{
			m_polarCurve->setPen(invisPen);
		}
	}

	// Setup points
	if (m_config.getPointSymbol() != Plot1DCurveCfg::NoSymbol)
	{
		if (m_config.getDimmed())
		{
			if (m_config.getLinePen().getStyle() == LineStyle::NoLine)
			{
				// Dimmed Point Pen & Brush if no line is shown
				if (m_cartesianCurvePointSymbol)
				{
					m_cartesianCurvePointSymbol->setPen(dimmedPen);
					m_cartesianCurvePointSymbol->setBrush(dimmedBrush);
				}
				if (m_polarCurvePointSymbol)
				{
					m_polarCurvePointSymbol->setPen(dimmedPen);
					m_polarCurvePointSymbol->setBrush(dimmedBrush);
				}
			}
			else
			{
				// Hide points if line is shown.
				if (m_cartesianCurvePointSymbol)
				{
					m_cartesianCurvePointSymbol->setPen(QPen(QBrush(Qt::NoBrush), 0., Qt::PenStyle::NoPen));
					m_cartesianCurvePointSymbol->setBrush(QBrush(Qt::NoBrush));
				}
				if (m_polarCurvePointSymbol)
				{
					m_polarCurvePointSymbol->setPen(QPen(QBrush(Qt::NoBrush), 0., Qt::PenStyle::NoPen));
					m_polarCurvePointSymbol->setBrush(QBrush(Qt::NoBrush));
				}
			}
		}
		else
		{
			// Regular Point Pen & Brush

			QPen pointOutlinePen = QtFactory::toQPen(pointOutlinePenCfg);
			QBrush pointOutlineFillBrush = QtFactory::toQBrush(m_config.getPointFillPainter());

			if (m_config.getPointColorFromCurve())
			{
				pointOutlineFillBrush = linePen.brush();
				pointOutlinePen.setBrush(pointOutlineFillBrush);
			}

			if (m_cartesianCurvePointSymbol)
			{
				m_cartesianCurvePointSymbol->setPen(pointOutlinePen);
				m_cartesianCurvePointSymbol->setBrush(pointOutlineFillBrush);
			}
			if (m_polarCurvePointSymbol)
			{
				m_polarCurvePointSymbol->setPen(pointOutlinePen);
				m_polarCurvePointSymbol->setBrush(pointOutlineFillBrush);
			}
		}

		// Symbol and size
		if (m_cartesianCurvePointSymbol)
		{
			m_cartesianCurvePointSymbol->setStyle(toQwtSymbolStyle(m_config.getPointSymbol()));
			m_cartesianCurvePointSymbol->setSize(m_config.getPointSize());
		}
		if (m_polarCurvePointSymbol)
		{
			m_polarCurvePointSymbol->setStyle(toQwtSymbolStyle(m_config.getPointSymbol()));
			m_polarCurvePointSymbol->setSize(m_config.getPointSize());
		}
	}
	else
	{ 
		// No symbol
		if (m_cartesianCurvePointSymbol)
		{
			m_cartesianCurvePointSymbol->setStyle(QwtSymbol::NoSymbol);
		}
		if (m_polarCurvePointSymbol)
		{
			m_polarCurvePointSymbol->setStyle(QwtSymbol::NoSymbol);
		}
	}

	if (m_cartesianCurve)
	{
		m_cartesianCurve->setTitle(QString::fromStdString(m_config.getTitle()));
		m_cartesianCurve->setVisible(m_config.getVisible());
		m_cartesianCurve->setPointInterval(m_config.getPointInterval());
	}
	if (m_polarCurve)
	{
		m_polarCurve->setTitle(QString::fromStdString(m_config.getTitle()));
		m_polarCurve->setVisible(m_config.getVisible());
		m_polarCurve->setPointInterval(m_config.getPointInterval());
	}

	m_data.resetCachedRect();

	// Update/Create legend item
	updateLegendVisualization();
}

void ot::PlotDataset::buildCartesianCurve()
{
	if (m_cartesianCurve == nullptr)
	{
		OTAssert(m_cartesianCurvePointSymbol == nullptr, "Cartesian curve point symbol should be null when cartesian curve is null");

		m_cartesianCurve = new CartesianPlotCurve(QString::fromStdString(m_config.getTitle()));
		m_cartesianCurvePointSymbol = new QwtSymbol();
		m_cartesianCurve->setSymbol(m_cartesianCurvePointSymbol);
		m_cartesianCurve->setSamples(m_data.getCartesianAccessor());
	}
}

void ot::PlotDataset::buildPolarCurve()
{
	if (m_polarCurve == nullptr)
	{
		OTAssert(m_polarCurvePointSymbol == nullptr, "Polar curve point symbol should be null when polar curve is null");

		m_polarCurve = new PolarPlotCurve(QString::fromStdString(m_config.getTitle()));
		m_polarCurvePointSymbol = new QwtSymbol();
		m_polarCurve->setSymbol(m_polarCurvePointSymbol);
		m_polarCurve->setData(m_data.getPolarAccessor());
	}
}

void ot::PlotDataset::createLegendItem()
{
	if (!m_legendItem)
	{
		if (!m_ownerPlot)
		{
			OT_LOG_E("Can not create legend item without owner set");
			return;
		}
		else if (QThread::currentThread() != m_ownerPlot->thread())
		{
			OT_LOG_E("Can not create legend item from non main gui thread");
			return;
		}

		m_legendItem = new PlotLegendItem(this);
		m_legendItem->setLegend(m_ownerPlot->getLegend());
		m_legendItem->setLabel(QString::fromStdString(m_config.getEntityName()));
	}
}

void ot::PlotDataset::updateLegendVisualization()
{
	if (!m_legendItem)
	{
		return;
	}

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	const ColorStyleValue& dimmedColorValue = cs.getValue(ColorStyleValueEntry::PlotCurveDimmed);

	m_legendItem->setLabel(QString::fromStdString(m_config.getEntityName()));
	if (m_config.getToolTip().empty())
	{
		m_legendItem->setToolTip(QString::fromStdString(m_config.getTitle()));
	}
	else
	{
		m_legendItem->setToolTip(QString::fromStdString(m_config.getToolTip()));
	}
	m_legendItem->setSelectedPainter(m_config.getLinePen().getPainter());
	m_legendItem->setDimmedPainter(dimmedColorValue.painter());
	m_legendItem->updateVisibility();
}
