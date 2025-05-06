//! @file PlotBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PlotBase.h"
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
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>

// std header
#include <memory>

ot::PlotBase::PlotBase() :
	m_isError(false), m_currentPlotType(Plot1DCfg::Cartesian)
{
	m_centralWidget = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralWidget);

	m_errorLabel = new QLabel("Error");
	m_errorLabel->setVisible(false);

	// Create plots
	m_cartesianPlot = new CartesianPlot(this);
	m_polarPlot = new PolarPlot(this);

	m_polarPlot->setParent(nullptr);
	m_polarPlot->setVisible(false);
	m_centralLayout->addWidget(m_cartesianPlot);

}

ot::PlotBase::~PlotBase() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

ot::AbstractPlot* ot::PlotBase::getPlot()
{
	if (m_currentPlotType == Plot1DCfg::PlotType::Cartesian)
	{
		return m_cartesianPlot;
	}
	else
	{
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
			m_centralLayout->removeWidget(m_polarPlot);
			m_polarPlot->setParent(nullptr);
			m_polarPlot->setVisible(false);
			m_centralLayout->addWidget(m_cartesianPlot);
			m_cartesianPlot->setVisible(true);
			break;

		case Plot1DCfg::Polar:
			m_centralLayout->removeWidget(m_cartesianPlot);
			m_cartesianPlot->setParent(nullptr);
			m_cartesianPlot->setVisible(false);			
			m_centralLayout->addWidget(m_polarPlot);
			m_polarPlot->setVisible(true);
			break;

		default:
			OT_LOG_EAS("Unknown plot type (" + std::to_string((int)m_currentPlotType) + ")");
			return;
		}
	}
}

void ot::PlotBase::resetView(void) {
	m_cartesianPlot->resetPlotView();
	m_polarPlot->resetPlotView();
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
		currentPlot = m_cartesianPlot;
		break;

	case Plot1DCfg::Polar:
		currentPlot = m_polarPlot;
		break;

	default:
		OT_LOG_EAS("Unknown plot type (" + std::to_string((int)m_currentPlotType) + ")");
		return;
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

void ot::PlotBase::setIncompatibleData(void) {
	clear(false);
	setErrorState(true, "Incompatible data");
}

void ot::PlotBase::refresh(void) {
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

void ot::PlotBase::datasetSelectionChanged(PlotDataset * _selectedDataset) {
	bool ctrlPressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);

	if (_selectedDataset == nullptr) {
		if (!ctrlPressed) {
			Q_EMIT resetItemSelectionRequest();
		}
	}
	else {
		Q_EMIT setItemSelectedRequest(_selectedDataset->getNavigationId(), ctrlPressed);
	}
}

void ot::PlotBase::applyConfig(void) {
	m_cartesianPlot->setTitle(m_config.getTitle().c_str());
	m_polarPlot->setTitle(m_config.getTitle().c_str());

	this->setPlotType(m_config.getPlotType());

	// Ensure that the axis titles are compatible
	bool compatible = true;

	std::string axisTitleX;
	std::string axisTitleY;

	std::list<PlotDataset*> cache = this->getAllDatasets();
	if (!cache.empty()) {
		const ot::Plot1DCurveCfg& firstCfg = cache.front()->getConfig();
		axisTitleX = createAxisLabel(firstCfg.getXAxisTitle(), firstCfg.getXAxisUnit());
		axisTitleY = createAxisLabel(firstCfg.getYAxisTitle(), firstCfg.getYAxisUnit());
		cache.pop_front();

		for (const PlotDataset* itm : cache) {
			const ot::Plot1DCurveCfg& currentCfg = itm->getConfig();
			if (axisTitleX != createAxisLabel(currentCfg.getXAxisTitle(), currentCfg.getXAxisUnit())) {
				compatible = false;
				break;
			}
			if (axisTitleY != createAxisLabel(currentCfg.getYAxisTitle(), currentCfg.getYAxisUnit())) {
				compatible = false;
				break;
			}
		}
	}

	if (compatible) {
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

	} // compatible

	else {
		this->setIncompatibleData();

	} // !compatible

	m_cartesianPlot->updateGrid();
	m_cartesianPlot->updateLegend();
	m_cartesianPlot->updateWholePlot();

	m_polarPlot->updateGrid();
	m_polarPlot->updateLegend();
	m_polarPlot->updateWholePlot();
}

std::string ot::PlotBase::createAxisLabel(const std::string& _axisTitle, std::string _unit)
{
	_unit.erase(std::remove_if(_unit.begin(), _unit.end(), isspace), _unit.end());	
	if (!_unit.empty())
	{
		return _axisTitle + " [" + _unit + "]";
	}
	return _axisTitle;
}
