// @otlicense
// File: PlotBase.cpp
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
#include "OTCore/String.h"
#include "OTCore/Symbol.h"
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlot.h"
#include "OTWidgets/Plot/Polar/PolarPlot.h"
#include "OTWidgets/Widgets/Label.h"

// Qwt header
#include <qwt_plot_curve.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_canvas.h>
#include <qwt_symbol.h>

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>

// std header
#include <memory>

ot::PlotBase::PlotBase(QWidget* _parent) :
	m_isError(false), m_currentPlotType(Plot1DCfg::Cartesian)
{
	m_centralWidget = new QWidget(_parent);
	QHBoxLayout* centralLayout = new QHBoxLayout(m_centralWidget);

	QVBoxLayout* plotContainerLayout = new QVBoxLayout;
	centralLayout->addLayout(plotContainerLayout, 1);
	plotContainerLayout->setContentsMargins(0, 0, 0, 0);

	m_plotLayout = new QVBoxLayout();
	plotContainerLayout->addLayout(m_plotLayout, 1);

	QHBoxLayout* infoLayout = new QHBoxLayout();
	infoLayout->setContentsMargins(2, 2, 2, 2);
	plotContainerLayout->addLayout(infoLayout, 0);

	m_infoLabel = new Label(m_centralWidget);
	infoLayout->addWidget(m_infoLabel);
	infoLayout->addStretch(1);

	m_errorLabel = new Label("Error", m_centralWidget);
	m_errorLabel->setVisible(false);

	// Create plots
	m_cartesianPlot = new CartesianPlot(this, nullptr);
	m_polarPlot = new PolarPlot(this, nullptr);

	m_plotLayout->addWidget(m_cartesianPlot->getQWidget());

	// Layout
	m_legendContainer = new QWidget(m_centralWidget);
	centralLayout->addWidget(m_legendContainer, 0);

	QVBoxLayout* legendContainerLayout = new QVBoxLayout(m_legendContainer);
	legendContainerLayout->setContentsMargins(0, 0, 0, 0);

	legendContainerLayout->addWidget(new Label(" ", m_legendContainer), 0);

	m_legend = new PlotLegend(this, m_legendContainer);
	m_legendContainer->setHidden(true);
	legendContainerLayout->addWidget(m_legend->getQWidget(), 1);
}

ot::PlotBase::~PlotBase() {
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

ot::AbstractPlot* ot::PlotBase::getPlot() {
	if (m_currentPlotType == Plot1DCfg::PlotType::Cartesian) {
		return m_cartesianPlot;
	}
	else {
		return m_polarPlot;
	}
}

void ot::PlotBase::setPlotType(Plot1DCfg::PlotType _type) {
	if (_type == m_currentPlotType) {
		return;
	}

	m_currentPlotType = _type;

	// Change the current plot widget if the plot is currently not in an error state
	if (!m_isError) {
		switch (m_currentPlotType)
		{
		case Plot1DCfg::Cartesian:
			m_plotLayout->removeWidget(m_polarPlot->getQWidget());
			m_polarPlot->setParent(nullptr);
			m_polarPlot->hide();

			m_plotLayout->addWidget(m_cartesianPlot->getQWidget());
			m_cartesianPlot->setParent(m_centralWidget);
			m_cartesianPlot->show();
			break;

		case Plot1DCfg::Polar:
			m_plotLayout->removeWidget(m_cartesianPlot->getQWidget());
			m_cartesianPlot->setParent(nullptr);
			m_cartesianPlot->hide();

			m_plotLayout->addWidget(m_polarPlot->getQWidget());
			m_polarPlot->setParent(m_centralWidget);
			m_polarPlot->show();
			break;

		default:
			OT_LOG_EAS("Unknown plot type (" + std::to_string((int)m_currentPlotType) + ")");
			return;
		}
	}

	for (auto data : getAllDatasets()) {
		data->rebuildCurve(true);
		data->updateCurveVisualization();
	}

	updateAxisTitles(true);
}

void ot::PlotBase::resetView() {
	m_cartesianPlot->resetPlotView();
	m_polarPlot->resetPlotView();
}

void ot::PlotBase::renameDataset(const std::string& _oldEntityPath, const std::string& _newEntityPath) {
	for (PlotDataset* data : this->getAllDatasets()) {
		if (data->getEntityName() == _oldEntityPath) {
			data->setEntityName(_newEntityPath);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

void ot::PlotBase::setErrorState(bool _isError, const QString & _message) 
{
	// Get the current or new plot widget
	// The widget depends on the current polot type
	QWidget * currentPlot;
	switch (m_currentPlotType)
	{
	case Plot1DCfg::Cartesian:
		currentPlot = m_cartesianPlot->getQWidget();
		break;

	case Plot1DCfg::Polar:
		currentPlot = m_polarPlot->getQWidget();
		break;

	default:
		OT_LOG_EAS("Unknown plot type (" + std::to_string((int)m_currentPlotType) + ")");
		return;
	}

	if (_isError && !m_isError) 
	{
		m_plotLayout->removeWidget(currentPlot);
		currentPlot->setParent(nullptr);
		m_errorLabel->setText(_message);
		m_plotLayout->addWidget(m_errorLabel);
		m_isError = true;
	}
	else if (!_isError && m_isError) 
	{
		m_plotLayout->removeWidget(m_errorLabel);
		m_errorLabel->setParent(nullptr);
		m_plotLayout->addWidget(currentPlot);
		m_isError = false;
	}
}

void ot::PlotBase::setIncompatibleData() 
{
	clear(false);
	setErrorState(true, "Incompatible data");
}

void ot::PlotBase::applyConfig()
{
	m_cartesianPlot->setTitle(m_config.getTitle().c_str());
	m_polarPlot->setTitle(m_config.getTitle().c_str());

	this->setPlotType(m_config.getPlotType());

	// Update quantities and scaling for all datasets
	const Plot1DAxisCfg::AxisQuantity xAxisQuantity = m_config.getXAxisQuantity();
	const Plot1DAxisCfg::AxisQuantity yAxisQuantity = m_config.getYAxisQuantity();
	const Plot1DAxisCfg::QuantityScaling xQuantityScaling = m_config.getXAxis().getQuantityScaling();
	const Plot1DAxisCfg::QuantityScaling yQuantityScaling = m_config.getYAxis().getQuantityScaling();

	for (PlotDataset* data : getAllDatasets())
	{
		data->setAxisQuantitiesAndScaling(xAxisQuantity, xQuantityScaling, yAxisQuantity, yQuantityScaling);
	}

	// Legend
	m_legendContainer->setHidden(!m_config.getLegendVisible());

	// Setup plot XY
	m_cartesianPlot->setPlotGridVisible(m_config.getGridVisible(), false);
	m_cartesianPlot->setPlotGridColor(m_config.getGridColor(), false);
	m_cartesianPlot->setPlotGridLineWidth(0.5, true);

	// Setup axis
	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsAutoScale());
	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsAutoScale());

	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsLogScale());
	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsLogScale());

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, m_config.getXAxisMax());
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, m_config.getXAxisMin());

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, m_config.getYAxisMax());
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, m_config.getYAxisMin());

	// Setup plot XY
	m_polarPlot->setPlotGridVisible(m_config.getGridVisible(), false);
	m_polarPlot->setPlotGridColor(m_config.getGridColor(), false);
	m_polarPlot->setPlotGridLineWidth(0.5, true);

	// Setup axis
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsAutoScale());
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsAutoScale());

	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsLogScale());
	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsLogScale());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, m_config.getXAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, m_config.getXAxisMin());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, m_config.getYAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, m_config.getYAxisMin());

	updateAxisTitles(false);

	m_cartesianPlot->updateGrid();
	m_cartesianPlot->updateLegend();
	m_cartesianPlot->updateWholePlot();
	m_cartesianPlot->update();

	m_polarPlot->updateGrid();
	m_polarPlot->updateLegend();
	m_polarPlot->updateWholePlot();
	m_polarPlot->update();
}

void ot::PlotBase::replot()
{
	m_polarPlot->replot();
	m_cartesianPlot->replot();
}

void ot::PlotBase::clear(bool _clearCache) 
{
	if (_clearCache) 
	{
		this->clearCache();
	}
	else 
	{
		this->detachAllCached();
	}

	m_cartesianPlot->setTitle("");
	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, "");
	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, "");

	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, false);
	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, false);

	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, false);
	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, false);

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, 1.0);
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, 0.0);

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, 1.0);
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, 0.0);

	m_cartesianPlot->updateWholePlot();

	m_polarPlot->setTitle("");
	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, "");
	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, "");

	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, false);
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, false);

	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, false);
	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, false);

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, 1.0);
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, 0.0);

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, 1.0);
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, 0.0);

	m_polarPlot->updateWholePlot();

	return;
}

void ot::PlotBase::setSelectedCurves(const UIDList& _selectedCurves) 
{
	for (PlotDataset* dataset : this->getAllDatasets()) 
	{
		if (std::find(_selectedCurves.begin(), _selectedCurves.end(), dataset->getEntityID()) != _selectedCurves.end()) 
		{
			dataset->setSelected(true);
		}
		else 
		{
			dataset->setSelected(false);
		}
	}
}

void ot::PlotBase::requestResetItemSelection() 
{
	Q_EMIT resetItemSelectionRequest();
}

void ot::PlotBase::requestCurveDoubleClicked(UID _entityID, bool _hasControlModifier) 
{
	Q_EMIT curveDoubleClicked(_entityID, _hasControlModifier);
}

void ot::PlotBase::setInfoText(const QString& _text) 
{
	m_infoText = _text;
	m_infoLabel->setText(m_infoText);
}

void ot::PlotBase::clearPositionInfoText()
{
	m_infoLabel->setText(m_infoText);
}

void ot::PlotBase::setInfoTextFromPosition(const QPoint& _pos)
{
	m_infoLabel->setText(toPositionInfoText(_pos, false));
}

void ot::PlotBase::setInfoTextFromPosition(const QPointF& _pos)
{
	m_infoLabel->setText(toPositionInfoText(_pos, false));
}

void ot::PlotBase::setInfoTextFromPosition(const QwtPointPolar& _pos)
{
	m_infoLabel->setText(toPositionInfoText(_pos, false));
}

QString ot::PlotBase::toPositionInfoText(const QPoint& _pos, bool _multiline) const
{
	return toPositionInfoText(_pos.toPointF(), false);
}

QString ot::PlotBase::toPositionInfoText(const QPointF& _pos, bool _multiline) const
{
	const std::string sep = _multiline ? "\n" : "     ";
	std::string txt = "X = " + m_config.getXAxis().getValueDisplayString(_pos.x(), m_config);
	txt.append(sep + "Y = " + m_config.getYAxis().getValueDisplayString(_pos.y(), m_config));
	return QString::fromStdString(txt);
}

QString ot::PlotBase::toPositionInfoText(const QwtPointPolar& _pos, bool _multiline) const
{
	const std::string sep = _multiline ? "\n" : "     ";
	std::string txt = "r = " + m_config.getXAxis().getValueDisplayString(_pos.radius(), m_config);
	txt.append(sep + std::string(Symbol::phi()) + " = " + m_config.getYAxis().getValueDisplayString(_pos.azimuth(), m_config, "deg "));
	txt.append(sep + std::string(Symbol::phi()) + " = " + m_config.getYAxis().getValueDisplayString(Math::degToRad(_pos.azimuth()), m_config, "rad "));
	return QString::fromStdString(txt);
}

void ot::PlotBase::updateAxisTitles(bool _replot)
{
	const QString axisTitleX = QString::fromStdString(m_config.getXAxisDisplayLabel());
	const QString axisTitleY = QString::fromStdString(m_config.getYAxisDisplayLabel());

	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX);
	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY);

	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX);
	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY);

	if (_replot) 
	{
		m_cartesianPlot->updateWholePlot();
		m_cartesianPlot->update();

		m_polarPlot->updateWholePlot();
		m_polarPlot->update();
	}
}
