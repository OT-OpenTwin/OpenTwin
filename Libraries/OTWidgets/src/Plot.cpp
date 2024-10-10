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

void ot::Plot::setFromConfig(const Plot1DCfg& _config) {
	// Detach all current data and refresh the title
	this->clear(false);

	m_cartesianPlot->setTitle(_config.getTitle().c_str());
	m_polarPlot->setTitle(_config.getTitle().c_str());

	this->setPlotType(_config.getPlotType());

	// Check cache
	std::list<Plot1DCurveCfg> entitiesToImport;
	for (const Plot1DCurveCfg& curveInfo : _config.getCurves()) {
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
	
	// Ensure that the axis titles are compatible
	bool compatible = true;

	std::string axisTitleX;
	std::string axisTitleY;

	if (!m_cache.empty())
	{
		axisTitleX = m_cache.begin()->second.second->getAxisTitleX();
		axisTitleY = m_cache.begin()->second.second->getAxisTitleY();	

		for (auto itm : m_cache) {
			if (axisTitleX != m_cache.begin()->second.second->getAxisTitleX()) compatible = false;
			if (axisTitleY != m_cache.begin()->second.second->getAxisTitleY()) compatible = false;
			if (!compatible) break;
		}
	}

	if (!compatible)
	{
		setIncompatibleData();
		return;
	}

	this->setAxisQuantity(_config.getAxisQuantity());
	
	// Setup plot XY
	m_cartesianPlot->setPlotGridVisible(_config.getGridVisible(), false);
	m_cartesianPlot->setPlotGridColor(_config.getGridColor(), false);
	m_cartesianPlot->setPlotGridLineWidth(0.5, true);

	m_cartesianPlot->setPlotLegendVisible(_config.getLegendVisible());

	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_cartesianPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

	// Setup axis
	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, _config.getXAxisIsAutoScale());
	m_cartesianPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, _config.getYAxisIsAutoScale());

	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, _config.getXAxisIsLogScale());
	m_cartesianPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, _config.getYAxisIsLogScale());

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, _config.getXAxisMax());
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, _config.getXAxisMin());

	m_cartesianPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, _config.getYAxisMax());
	m_cartesianPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, _config.getYAxisMin());

	m_cartesianPlot->updateGrid();
	m_cartesianPlot->updateLegend();

	m_cartesianPlot->updateWholePlot();

	// Setup plot XY
	m_polarPlot->setPlotGridVisible(_config.getGridVisible(), false);
	m_polarPlot->setPlotGridColor(_config.getGridColor(), false);
	m_polarPlot->setPlotGridLineWidth(0.5, true);

	m_polarPlot->setPlotLegendVisible(_config.getLegendVisible());

	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_polarPlot->setPlotAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

	// Setup axis
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::xBottom, _config.getXAxisIsAutoScale());
	m_polarPlot->setPlotAxisAutoScale(AbstractPlotAxis::yLeft, _config.getYAxisIsAutoScale());

	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::xBottom, _config.getXAxisIsLogScale());
	m_polarPlot->setPlotAxisLogScale(AbstractPlotAxis::yLeft, _config.getYAxisIsLogScale());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::xBottom, _config.getXAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::xBottom, _config.getXAxisMin());

	m_polarPlot->setPlotAxisMax(AbstractPlotAxis::yLeft, _config.getYAxisMax());
	m_polarPlot->setPlotAxisMin(AbstractPlotAxis::yLeft, _config.getYAxisMin());

	m_polarPlot->updateGrid();
	m_polarPlot->updateLegend();

	m_polarPlot->updateWholePlot();

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

void ot::Plot::setAxisQuantity(Plot1DCfg::AxisQuantity _quantity) {
	for (auto itm : m_cache) {
		itm.second.second->calculateData(_quantity);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions
/*
void ot::Plot::importData(const std::string & _projectName, const std::list<Plot1DCurveCfg>& _entitiesToImport) {
	if (_entitiesToImport.empty()) return;

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchCurves;
	for (auto item : _entitiesToImport)
	{
		prefetchCurves.push_back(std::pair<unsigned long long, unsigned long long>(item.getModelEntityID(), item.getModelEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurves);

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchCurveData;
	std::list< PlotDataset *> curves;

	for (auto item : _entitiesToImport)
	{
		unsigned long long entityID = item.getModelEntityID();
		unsigned long long entityVersion = item.getModelEntityVersion();

		EntityHandler handler;
		EntityBase* baseEntity = handler.readEntityFromEntityIDandVersion(entityID, entityVersion);
		std::unique_ptr<EntityResult1DCurve> curve(dynamic_cast<EntityResult1DCurve*>(baseEntity));

		// Read the curve data item
		unsigned long long curveDataStorageId = curve->getCurveDataStorageId();
		unsigned long long curveDataStorageVersion = curve->getCurveDataStorageVersion();
		baseEntity = handler.readEntityFromEntityIDandVersion(curveDataStorageId, curveDataStorageVersion);
		std::unique_ptr<EntityResult1DCurveData> curveData(dynamic_cast<EntityResult1DCurveData*>(baseEntity));
		std::string curveLabel = item.getName();

		ot::Color colour = curve->getColor();
		
		std::string xAxisLabel = curve->getAxisLabelX();
		std::string yAxisLabel = curve->getAxisLabelY();

		std::string xAxisUnit = curve->getUnitX();
		std::string yAxisUnit = curve->getUnitY();

		std::string axisTitleX = xAxisLabel + " [" + xAxisUnit + "]";
		std::string axisTitleY = yAxisLabel + " [" + yAxisUnit + "]";

		PlotDataset * newDataset = addDataset(curveLabel.c_str(), nullptr, nullptr, 0);

		newDataset->setEntityID(entityID);
		newDataset->setEntityVersion(entityVersion);
		newDataset->setCurveEntityID(curveDataStorageId);
		newDataset->setCurveEntityVersion(curveDataStorageVersion);
		newDataset->setTreeItemID(item.getTreeID());
		newDataset->setDimmed(item.isDimmed(), false);

		newDataset->setCurvePointsVisible(false, false);
		
		newDataset->setCurveColor(QColor(colour.r(), colour.g(), colour.b()), false);
		newDataset->setCurveTitle(curveLabel.c_str());
		newDataset->setAxisTitleX(axisTitleX);
		newDataset->setAxisTitleY(axisTitleY);

		newDataset->repaint();

		// Check whether we already have the curve data

		bool loadCurveDataRequired = true;

		if (m_cache.count(newDataset->getEntityID()) > 0)
		{
			PlotDataset * oldDataset = m_cache[newDataset->getEntityID()].second;

			if (oldDataset != nullptr)
			{
				if (oldDataset->getCurveEntityID() == newDataset->getCurveEntityID()
					&& oldDataset->getCurveEntityVersion() == newDataset->getCurveEntityVersion())
				{
					// The curve data in the previous data set is the same as the one in the new data set

					double *x = nullptr, *y = nullptr;
					long n;

					if (oldDataset->getCopyOfData(x, y, n))
					{
						newDataset->replaceData(x, y, n);
						loadCurveDataRequired = false;
						newDataset->attach();
					}
				}

			}
		}

		if (loadCurveDataRequired)
		{
			prefetchCurveData.push_back(std::pair<unsigned long long, unsigned long long>(curveDataStorageId, curveDataStorageVersion));

			curves.push_back(newDataset);
		}

		removeFromCache(newDataset->getEntityID());

		m_cache.insert_or_assign(newDataset->getEntityID(), std::pair<unsigned long long, PlotDataset *>(newDataset->getEntityVersion(), newDataset));
	}

	if (prefetchCurveData.empty()) return; // Nothing to load

	// Now load the curve data
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchCurveData);

	for (auto item : curves)
	{
		// Here we get the storage data of the underlying curve data
		unsigned long long entityID = item->getCurveEntityID();
		unsigned long long entityVersion = item->getCurveEntityVersion();

		auto doc = bsoncxx::builder::basic::document{};

		if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, entityVersion, doc))
		{
			assert(0);
			return;
		}

		auto doc_view = doc.view()["Found"].get_document().view();

		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

		EntityResult1DCurveData curveDataEntName(0, nullptr, nullptr, nullptr, nullptr, "");
		if (entityType != curveDataEntName.getClassName())
		{
			assert(0);
			return;
		}

		std::string schemaVersionField = "SchemaVersion_" + curveDataEntName.getClassName();
		int schemaVersion = (int)DataBase::GetIntFromView(doc_view, schemaVersionField.c_str());
		if (schemaVersion != 1)
		{
			assert(0);
			return;
		}

		auto arrayX = doc_view["dataX"].get_array().value;
		auto arrayYre = doc_view["dataY"].get_array().value;
		auto arrayYim = doc_view["dataYim"].get_array().value;

		size_t nX = std::distance(arrayX.begin(), arrayX.end());
		size_t nYre = std::distance(arrayYre.begin(), arrayYre.end());
		size_t nYim = std::distance(arrayYim.begin(), arrayYim.end());

		double * x = new double[nX];
		auto iX = arrayX.begin();

		for (unsigned long index = 0; index < nX; index++)
		{
			x[index] = iX->get_double();
			iX++;
		}

		double *yre = nullptr, *yim = nullptr;

		if (nYre > 0)
		{
			yre = new double[nYre];

			auto iYre = arrayYre.begin();

			for (unsigned long index = 0; index < nYre; index++)
			{
				yre[index] = iYre->get_double();
				iYre++;
			}
		}
		else {
			yre = new double[nX];
			for (unsigned long index = 0; index < nX; index++)
			{
				yre[index] = 0;
			}
		}

		// First we set the new data
		item->replaceData(x, yre, nX);

		if (nYim > 0)
		{
			yim = new double[nYim];

			auto iYim = arrayYim.begin();

			for (unsigned long index = 0; index < nYim; index++)
			{
				yim[index] = iYim->get_double();
				iYim++;
			}

			// Set the imaginary values
			assert(nX == nYre);
		}
		else {
			yim = new double[nX];
			for (unsigned long index = 0; index < nX; index++)
			{
				yim[index] = 0;
			}
		}

		item->setYim(yim);

		// Attatch the curve to the plot
		item->attach();
	}
}
*/