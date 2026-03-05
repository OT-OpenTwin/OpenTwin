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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/CartesianPlot.h"

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
	QVBoxLayout* centralLayout = new QVBoxLayout(m_centralWidget);
	centralLayout->setContentsMargins(0, 0, 0, 0);

	m_plotLayout = new QVBoxLayout();
	centralLayout->addLayout(m_plotLayout, 1);

	QHBoxLayout* infoLayout = new QHBoxLayout();
	infoLayout->setContentsMargins(2, 2, 2, 2);
	centralLayout->addLayout(infoLayout, 0);

	m_infoLabel = new Label(m_centralWidget);
	infoLayout->addWidget(m_infoLabel);
	infoLayout->addStretch(1);

	m_errorLabel = new Label("Error", m_centralWidget);
	m_errorLabel->setVisible(false);

	// Create plots
	m_cartesianPlot = new CartesianPlot(this, nullptr);
	m_polarPlot = new PolarPlot(this, nullptr);

	m_plotLayout->addWidget(m_cartesianPlot->getQWidget());
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

void ot::PlotBase::setErrorState(bool _isError, const QString & _message) {
	
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

	if (_isError && !m_isError) {
		
		m_plotLayout->removeWidget(currentPlot);
		currentPlot->setParent(nullptr);
		m_errorLabel->setText(_message);
		m_plotLayout->addWidget(m_errorLabel);
		m_isError = true;
	}
	else if (!_isError && m_isError) {
		m_plotLayout->removeWidget(m_errorLabel);
		m_errorLabel->setParent(nullptr);
		m_plotLayout->addWidget(currentPlot);
		m_isError = false;
	}
}

void ot::PlotBase::setIncompatibleData() {
	clear(false);
	setErrorState(true, "Incompatible data");
}

void ot::PlotBase::refresh() {
	this->applyConfig();
}

void ot::PlotBase::clear(bool _clearCache) {
	if (_clearCache) {
		this->clearCache();
	}
	else {
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

void ot::PlotBase::setSelectedCurves(const UIDList& _selectedCurves) {
	for (PlotDataset* dataset : this->getAllDatasets()) {
		if (std::find(_selectedCurves.begin(), _selectedCurves.end(), dataset->getEntityID()) != _selectedCurves.end()) {
			dataset->setSelected(true);
		}
		else {
			dataset->setSelected(false);
		}
	}
}

void ot::PlotBase::requestResetItemSelection() {
	Q_EMIT resetItemSelectionRequest();
}

void ot::PlotBase::requestCurveDoubleClicked(UID _entityID, bool _hasControlModifier) {
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
	const QString posText = "x = " + QString::number(_pos.x()) + "; y = " + QString::number(_pos.y());
	m_infoLabel->setText(posText);
}

void ot::PlotBase::setInfoTextFromPosition(const QPointF& _pos)
{
	const QString posText = "x = " + QString::number(_pos.x()) + "; y = " + QString::number(_pos.y());
	m_infoLabel->setText(posText);
}

void ot::PlotBase::setInfoTextFromPosition(const QwtPointPolar& _pos)
{
	const QString posText = "r = " + QString::number(_pos.radius()) + "    \xCF\x86 = " + QString::number(qRadiansToDegrees(_pos.azimuth())) + " deg    \xCF\x86 = " + QString::number(_pos.azimuth()) + " rad";
	m_infoLabel->setText(posText);
}

void ot::PlotBase::applyConfig() {
	m_cartesianPlot->setTitle(m_config.getTitle().c_str());
	m_polarPlot->setTitle(m_config.getTitle().c_str());

	this->setPlotType(m_config.getPlotType());

	std::string axisTitleX;
	std::string axisTitleY;
		
	axisTitleX = m_config.getAxisLabelX();
	axisTitleY = m_config.getAxisLabelY();
	
	// Update quantities
	for (auto data : getAllDatasets()) {
		data->setAxisQuantities(m_config.getXAxisQuantity(), m_config.getYAxisQuantity());
	}

	// Setup plot XY
	m_cartesianPlot->setPlotGridVisible(m_config.getGridVisible(), false);
	m_cartesianPlot->setPlotGridColor(m_config.getGridColor(), false);
	m_cartesianPlot->setPlotGridLineWidth(0.5, true);

	m_cartesianPlot->setPlotLegendVisible(m_config.getLegendVisible());

	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

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

	m_polarPlot->setPlotLegendVisible(m_config.getLegendVisible());

	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

	// Setup axis
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsAutoScale());
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsAutoScale());

	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, m_config.getXAxisIsLogScale());
	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, m_config.getYAxisIsLogScale());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, m_config.getXAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, m_config.getXAxisMin());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, m_config.getYAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, m_config.getYAxisMin());

	m_cartesianPlot->updateGrid();
	m_cartesianPlot->updateLegend();
	m_cartesianPlot->updateWholePlot();
	m_cartesianPlot->update();

	m_polarPlot->updateGrid();
	m_polarPlot->updateLegend();
	m_polarPlot->updateWholePlot();
	m_polarPlot->update();
}
