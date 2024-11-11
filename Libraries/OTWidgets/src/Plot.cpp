//! @file Plot.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Plot.h"
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

ot::Plot::Plot()
	: m_currentDatasetId(0), m_isError(false), m_currentPlotType(Plot1DCfg::Cartesian)
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

ot::Plot::~Plot() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

void ot::Plot::setPlotType(Plot1DCfg::PlotType _type) {
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

ot::PlotDataset * ot::Plot::addDataset(
	const QString &			_title,
	double *				_dataX,
	double *				_dataY,
	long					_dataSize
) {
	return new PlotDataset(this, ++m_currentDatasetId, _title, _dataX, _dataY, _dataSize);
}

void ot::Plot::setFromDataBaseConfig(const Plot1DDataBaseCfg& _config) {
	// Detach all current data and refresh the title
	this->clear(false);

	m_config = _config;

	// Check cache
	std::list<Plot1DCurveInfoCfg> entitiesToImport;
	for (const Plot1DCurveInfoCfg& curveInfo : _config.getCurves()) {
		// Check for entity ID
		auto itm = m_cache.find(curveInfo.getId());
		if (itm != m_cache.end()) {
			// Check if the entity Versions match
			if (itm->second.first != curveInfo.getVersion()) {
				entitiesToImport.push_back(curveInfo);
			}
			else {
				itm->second.second->setTreeItemID(curveInfo.getTreeId());  // We need to update the tree id, since this might have changed due to undo / redo or open project
				itm->second.second->setDimmed(curveInfo.getDimmed(), true);
				itm->second.second->attach();
			}
		}
		else {
			entitiesToImport.push_back(curveInfo);
		}
	}

	// Import remaining entities
	this->importData(_config.getProjectName(), entitiesToImport);

	this->applyConfig();
}

void ot::Plot::resetView(void) {
	assert(0);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

void ot::Plot::setErrorState(bool _isError, const QString & _message) {
	
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

void ot::Plot::setIncompatibleData(void) {
	clear(false);
	setErrorState(true, "Incompatible data");
}

void ot::Plot::refresh(void) {
	this->applyConfig();
}

void ot::Plot::clear(bool _clearCache) {
	if (_clearCache) {
		for (auto itm : m_cache) {
			delete itm.second.second;
		}
		m_cache.clear();
	}
	else {
		for (auto itm : m_cache) {
			itm.second.second->detach();
		}
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

void ot::Plot::removeFromCache(unsigned long long _entityID) {
	auto itm = m_cache.find(_entityID);
	if (itm != m_cache.end()) {
		delete itm->second.second;
		m_cache.erase(_entityID);
	}
}

void ot::Plot::datasetSelectionChanged(PlotDataset * _selectedDataset) {
	bool ctrlPressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);

	if (_selectedDataset == nullptr) {
		if (!ctrlPressed) {
			Q_EMIT resetItemSelectionRequest();
		}
	}
	else {
		Q_EMIT setItemSelectedRequest(_selectedDataset->getTreeItemID(), ctrlPressed);
	}
}

ot::PlotDataset* ot::Plot::findDataset(QwtPlotCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_cartesianCurve == _curve) {
			return itm.second.second;
		}
	}
	return nullptr;
}

ot::PlotDataset* ot::Plot::findDataset(QwtPolarCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_polarCurve == _curve) {
			return itm.second.second;
		}
	}
	return nullptr;
}

ot::PlotDataset* ot::Plot::findDataset(UID _entityID) {
	auto it = m_cache.find(_entityID);
	if (it == m_cache.end()) {
		return nullptr;
	}
	else {
		return it->second.second;
	}
}

void ot::Plot::setAxisQuantity(Plot1DCfg::AxisQuantity _quantity) {
	for (auto itm : m_cache) {
		itm.second.second->calculateData(_quantity);
	}
}

bool ot::Plot::hasCachedEntity(UID _entityID) const {
	return m_cache.find(_entityID) != m_cache.end();
}

bool ot::Plot::changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion) {
	auto it = m_cache.find(_entityID);
	if (it == m_cache.end()) {
		return false;
	}

	if (it->second.first == _newEntityVersion) {
		return false;
	}
	else {
		it->second.first = _newEntityVersion;
		it->second.second->setCurveEntityVersion(_newEntityVersion);
		return true;
	}
}

std::list<ot::PlotDataset*> ot::Plot::getDatasets(void) const {
	std::list<PlotDataset*> result;

	for (const auto& it : m_cache) {
		result.push_back(it.second.second);
	}

	return result;
}

void ot::Plot::applyConfig(void) {
	m_cartesianPlot->setTitle(m_config.getTitle().c_str());
	m_polarPlot->setTitle(m_config.getTitle().c_str());

	this->setPlotType(m_config.getPlotType());

	// Ensure that the axis titles are compatible
	bool compatible = true;

	std::string axisTitleX;
	std::string axisTitleY;

	if (!m_cache.empty()) {
		axisTitleX = m_cache.begin()->second.second->getAxisTitleX();
		axisTitleY = m_cache.begin()->second.second->getAxisTitleY();

		for (const auto& itm : m_cache) {
			if (axisTitleX != m_cache.begin()->second.second->getAxisTitleX()) compatible = false;
			if (axisTitleY != m_cache.begin()->second.second->getAxisTitleY()) compatible = false;
			if (!compatible) break;
		}
	}

	if (!compatible) {
		setIncompatibleData();
		return;
	}

	this->setAxisQuantity(m_config.getAxisQuantity());

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

	m_cartesianPlot->updateGrid();
	m_cartesianPlot->updateLegend();

	m_cartesianPlot->updateWholePlot();

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

	m_polarPlot->updateGrid();
	m_polarPlot->updateLegend();

	m_polarPlot->updateWholePlot();
}