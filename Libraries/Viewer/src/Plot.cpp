#include "stdafx.h"

#include "Plot.h"
#include "xyPlot.h"
#include "polarPlot.h"
#include "DataBase.h"
#include "Viewer.h"

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qapplication.h>

#include <qwt_plot_curve.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_canvas.h>
#include <qwt_symbol.h>

#include "EntityHandler.h"
#include "EntityResult1DCurve.h"
#include "EntityResult1DCurveData.h"

#include <memory>
const double c_pi{ 3.14159265358979323846 };

Plot::Plot(Viewer * _viewer)
	: m_currentDatasetId{ 0 }, m_isError{ false }, m_currentPlot{ AbstractPlot::Cartesian }, m_viewer(_viewer)
{
	m_centralWidget = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralWidget);

	m_errorLabel = new QLabel("Error");
	m_errorLabel->setVisible(false);

	// Create plots
	m_xyPlot = new xyPlot(this);
	m_polarPlot = new polarPlot(this);

	m_polarPlot->setParent(nullptr);
	m_polarPlot->setVisible(false);
	m_centralLayout->addWidget(m_xyPlot);

}

Plot::~Plot() {

}


// ###########################################################################

// Setter

void Plot::setPlotType(AbstractPlot::PlotType _type) {
	if (_type == m_currentPlot) { return; }

	m_currentPlot = _type;

	// Change the current plot widget if the plot is currently not in an error state
	if (!m_isError) {
		switch (m_currentPlot)
		{
		case AbstractPlot::Cartesian:
			m_centralLayout->removeWidget(m_polarPlot);
			m_polarPlot->setParent(nullptr);
			m_polarPlot->setVisible(false);
			m_centralLayout->addWidget(m_xyPlot);
			m_xyPlot->setVisible(true);
			break;
		case AbstractPlot::Polar:
			m_centralLayout->removeWidget(m_xyPlot);
			m_xyPlot->setParent(nullptr);
			m_xyPlot->setVisible(false);			
			m_centralLayout->addWidget(m_polarPlot);
			m_polarPlot->setVisible(true);
			break;
		default: assert(0); return;
		}
	}
}

PlotDataset * Plot::addDataset(
	const QString &			_title,
	double *				_dataX,
	double *				_dataY,
	long					_dataSize
) {
	return new PlotDataset(this, m_xyPlot, m_polarPlot, ++m_currentDatasetId, _title, _dataX, _dataY, _dataSize);
}

void Plot::setFromDataBase(
	AbstractPlot::PlotType				_plotType,
	PlotDataset::axisQuantity			_axisType,
	const std::string &					_title,
	bool								_grid,
	int									_gridColor[],
	bool								_legend,
	bool								_logscaleX,
	bool								_logscaleY,
	bool								_autoscaleX,
	bool								_autoscaleY,
	double								_xmin,
	double								_xmax,
	double								_ymin,
	double								_ymax,
	const std::string &					_projectName,
	const std::list<PlotCurveItem> &	_entityIDandVersions
) {
	// Detach all current data and refresh the title
	clear(false);

	m_xyPlot->setTitle(_title.c_str());
	m_polarPlot->setTitle(_title.c_str());

	setPlotType(_plotType);

	// Check cache
	std::list<PlotCurveItem> entitiesToImport;
	for (auto e : _entityIDandVersions) {
		// Check for entity ID
		auto itm = m_cache.find(e.getModelEntityID());
		if (itm != m_cache.end()) {
			// Check if the entity Versions match
			if (itm->second.first != e.getModelEntityVersion()) {
				entitiesToImport.push_back(e);
			}
			else {
				itm->second.second->setTreeItemID(e.getTreeID());  // We need to update the tree id, since this might have changed due to undo / redo or open project
				itm->second.second->setDimmed(e.isDimmed(), true);
				itm->second.second->attach();
			}
		}
		else {
			entitiesToImport.push_back(e);
		}
	}

	// Import remaining entities
	setFromDataBase(_projectName, entitiesToImport);
	/*
	auto d = new PlotDataset(this, m_xyPlot, m_polarPlot, 1, "Test", nullptr, nullptr, 0);

	double * vx = new double[100];
	double * vy = new double[100];
	for (int i = 0; i < 100; i++) {
		vx[i] = i;
		vy[i] = i * i;
	}
	d->replaceData(vx, vy, 100);
	d->repaint();
	d->attach();*/

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

	setAxisQuantity(_axisType);
	
	// Setup plot XY
	m_xyPlot->SetGridVisible(_grid, false);
	m_xyPlot->SetGridColor(QColor(_gridColor[0], _gridColor[1], _gridColor[2]), false);
	m_xyPlot->SetGridLineWidth(0.5, true);

	m_xyPlot->SetLegendVisible(_legend);

	m_xyPlot->SetAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_xyPlot->SetAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

	// Setup axis
	m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, _autoscaleX);
	m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, _autoscaleY);

	m_xyPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, _logscaleX);
	m_xyPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, _logscaleY);

	m_xyPlot->SetAxisMax(AbstractPlotAxis::xBottom, _xmax);
	m_xyPlot->SetAxisMin(AbstractPlotAxis::xBottom, _xmin);

	m_xyPlot->SetAxisMax(AbstractPlotAxis::yLeft, _ymax);
	m_xyPlot->SetAxisMin(AbstractPlotAxis::yLeft, _ymin);

	m_xyPlot->RepaintGrid();
	m_xyPlot->RepaintLegend();

	m_xyPlot->RefreshWholePlot();

	// Setup plot XY
	m_polarPlot->SetGridVisible(_grid, false);
	m_polarPlot->SetGridColor(QColor(_gridColor[0], _gridColor[1], _gridColor[2]), false);
	m_polarPlot->SetGridLineWidth(0.5, true);

	m_polarPlot->SetLegendVisible(_legend);

	m_polarPlot->SetAxisTitle(AbstractPlotAxis::xBottom, axisTitleX.c_str());
	m_polarPlot->SetAxisTitle(AbstractPlotAxis::yLeft, axisTitleY.c_str());

	// Setup axis
	m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, _autoscaleX);
	m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, _autoscaleY);

	m_polarPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, _logscaleX);
	m_polarPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, _logscaleY);

	m_polarPlot->SetAxisMax(AbstractPlotAxis::xBottom, _xmax);
	m_polarPlot->SetAxisMin(AbstractPlotAxis::xBottom, _xmin);

	m_polarPlot->SetAxisMax(AbstractPlotAxis::yLeft, _ymax);
	m_polarPlot->SetAxisMin(AbstractPlotAxis::yLeft, _ymin);

	m_polarPlot->RepaintGrid();
	m_polarPlot->RepaintLegend();

	m_polarPlot->RefreshWholePlot();

}

void Plot::resetView(void) {
	assert(0);
}

// ###########################################################################

// Data handling

void Plot::setErrorState(bool _isError, const QString & _message) {
	
	// Get the current or new plot widget
	// The widget depends on the current polot type
	QWidget * currentPlot;
	switch (m_currentPlot)
	{
	case AbstractPlot::Cartesian: currentPlot = m_xyPlot; break;
	case AbstractPlot::Polar: currentPlot = m_polarPlot; break;
	default: assert(0); return;
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

void Plot::setIncompatibleData(void) {
	clear(false);
	setErrorState(true, "Incompatible data");
}

void Plot::refresh(void) {

}

void Plot::clear(bool _clearCache) {
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

	m_xyPlot->setTitle("");
	m_xyPlot->SetAxisTitle(AbstractPlotAxis::xBottom, "");
	m_xyPlot->SetAxisTitle(AbstractPlotAxis::yLeft, "");

	m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, false);
	m_xyPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, false);

	m_xyPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, false);
	m_xyPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, false);

	m_xyPlot->SetAxisMax(AbstractPlotAxis::xBottom, 1.0);
	m_xyPlot->SetAxisMin(AbstractPlotAxis::xBottom, 0.0);

	m_xyPlot->SetAxisMax(AbstractPlotAxis::yLeft, 1.0);
	m_xyPlot->SetAxisMin(AbstractPlotAxis::yLeft, 0.0);

	m_xyPlot->RefreshWholePlot();

	m_polarPlot->setTitle("");
	m_polarPlot->SetAxisTitle(AbstractPlotAxis::xBottom, "");
	m_polarPlot->SetAxisTitle(AbstractPlotAxis::yLeft, "");

	m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::xBottom, false);
	m_polarPlot->SetAxisAutoScale(AbstractPlotAxis::yLeft, false);

	m_polarPlot->SetAxisLogScale(AbstractPlotAxis::xBottom, false);
	m_polarPlot->SetAxisLogScale(AbstractPlotAxis::yLeft, false);

	m_polarPlot->SetAxisMax(AbstractPlotAxis::xBottom, 1.0);
	m_polarPlot->SetAxisMin(AbstractPlotAxis::xBottom, 0.0);

	m_polarPlot->SetAxisMax(AbstractPlotAxis::yLeft, 1.0);
	m_polarPlot->SetAxisMin(AbstractPlotAxis::yLeft, 0.0);

	m_polarPlot->RefreshWholePlot();

	return;
}

void Plot::removeFromCache(unsigned long long _entityID) {
	auto itm = m_cache.find(_entityID);
	if (itm != m_cache.end()) {
		delete itm->second.second;
		m_cache.erase(_entityID);
	}
}

void Plot::datasetSelectionChanged(PlotDataset * _selectedDataset) {
	if (m_viewer == nullptr) return;

	bool ctrlPressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);

	if (_selectedDataset == nullptr)
	{
		if (!ctrlPressed)
		{
			m_viewer->reset1DPlotItemSelection();
		}
	}
	else
	{
		m_viewer->set1DPlotItemSelected(_selectedDataset->getTreeItemID(), ctrlPressed);
	}
}

PlotDataset * Plot::findDataset(QwtPlotCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_xyCurve == _curve) { return itm.second.second; }
	}
	return nullptr;
}

PlotDataset * Plot::findDataset(QwtPolarCurve * _curve) {
	for (auto itm : m_cache) {
		if (itm.second.second->m_polarCurve == _curve) { return itm.second.second; }
	}
	return nullptr;
}

void Plot::setAxisQuantity(PlotDataset::axisQuantity _type) {
	for (auto itm : m_cache) {
		itm.second.second->calculateData(_type);
	}
}

// ########################################################################################

// Private functions

void Plot::setFromDataBase(const std::string & _projectName, const std::list<PlotCurveItem> & _entitiesToImport) {
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

// ########################################################################################

// ########################################################################################

// ########################################################################################

std::string PlotDataset::plotQuantityString(axisQuantity _quantity) {
	switch (_quantity)
	{
	case PlotDataset::Magnitude: return "Magnitude";
	case PlotDataset::Phase: return "Phase";
	case PlotDataset::Real: return "Real";
	case PlotDataset::Imaginary: return "Imaginary";
	default:
		assert(0);
		return "Real";
	}
}

PlotDataset::axisQuantity PlotDataset::plotQuantityFromString(const std::string& _string) {
	if (_string == "Magnitude") { return PlotDataset::Magnitude; }
	else if (_string == "Phase") { return PlotDataset::Phase; }
	else if (_string == "Real") { return PlotDataset::Real; }
	else if (_string == "Imaginary") { return PlotDataset::Imaginary; }
	else {
		assert(0);
		return PlotDataset::Real;
	}
}

PlotDataset::PlotDataset(
	Plot *					_owner,
	xyPlot *				_xyPlot,
	polarPlot *				_polarPlot,
	int						_id,
	const QString &			_title,
	double *				_dataX,
	double *				_dataY,
	long					_dataSize
)
	: m_id(_id), m_dataSize(_dataSize), m_dataX(_dataX), m_dataY(_dataY),
	m_owner(_owner), m_xyPlot(_xyPlot), m_polarPlot(_polarPlot), m_curveTitle(_title), m_isAttatched(false),
	m_curveColor(255, 0, 0), m_curvePenSize(1.0), m_isVisible(true), m_isDimmed(false), m_curvePointInnerColor(0, 255, 0),
	m_curvePointOutterColor(0, 255, 0), m_curvePointOutterColorWidth(1.0), m_curvePointSize(5), m_dataYim(nullptr), m_dataYcalc(nullptr),
	m_polarData(nullptr), m_dataXcalc(nullptr)
{
	m_xyCurve = new QwtPlotCurve(_title);
	m_xyCurvePointSymbol = new QwtSymbol;
	m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarCurve = new QwtPolarCurve(_title);
	m_polarCurvePointSymbol = new QwtSymbol;
	m_polarData = new PolarPlotData(m_dataX, m_dataY, _dataSize);
	m_polarCurve->setSymbol(m_polarCurvePointSymbol);
	m_polarCurve->setData(m_polarData);
}

PlotDataset::~PlotDataset() {
	detach();
	memFree();
}

// ###########################################################################

// Setter

void PlotDataset::replaceData(double * _dataX, double * _dataY, long _dataSize) {
	memFree();

	m_dataSize = _dataSize;
	m_dataX = _dataX;
	m_dataY = _dataY;

	m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);

	m_polarData->replaceData(m_dataX, m_dataY, m_dataSize);
	m_polarCurve->setData(m_polarData);
}

void PlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint) {
	m_isVisible = true;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurveWidth(double _penSize, bool _repaint) {
	m_curvePenSize = _penSize;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurveColor(const QColor & _color, bool _repaint) {
	m_curveColor = _color;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurvePointsVisible(bool _isVisible, bool _repaint) {
	m_curvePointsVisible = _isVisible;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurvePointInnerColor(const QColor & _color, bool _repaint) {
	m_curvePointInnerColor = _color;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurvePointOuterColor(const QColor & _color, bool _repaint) {
	m_curvePointOutterColor = _color;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurvePointSize(int _size, bool _repaint) {
	m_curvePointSize = _size;
	if (_repaint) { repaint(); }
}

void PlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint) {
	m_curvePointOutterColorWidth = _size;
	if (_repaint) { repaint(); }
}

void PlotDataset::attach(void) {
	if (m_isAttatched) { return; }
	m_isAttatched = true;

	m_xyCurve->attach(m_xyPlot);
	m_polarCurve->attach(m_polarPlot);
}

void PlotDataset::detach(void) {
	if (!m_isAttatched) { return; }
	m_isAttatched = false;

	m_xyCurve->detach();
	m_polarCurve->detach();
}

void PlotDataset::setCurveTitle(const QString & _title) {
	m_curveTitle = _title;
	m_xyCurve->setTitle(m_curveTitle);
	m_polarCurve->setTitle(m_curveTitle);
}

void PlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_isDimmed = _isDimmed;
	if (_repaint) { repaint(); }
}

void PlotDataset::calculateData(axisQuantity _complexType) {
	if (m_dataYcalc != nullptr) {
		delete[] m_dataYcalc;
		m_dataYcalc = nullptr;
	}
	if (m_dataXcalc != nullptr) {
		delete[] m_dataXcalc;
		m_dataXcalc = nullptr;
	}
	if (m_dataSize == 0) {
		return;
	}

	switch (_complexType)
	{
	case PlotDataset::Magnitude:
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}

		m_xyCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYcalc, m_dataSize);
		break;
	case PlotDataset::Phase:
		m_dataYcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataYcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / c_pi * 180;
		}
		m_xyCurve->setRawSamples(m_dataX, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYcalc, m_dataSize);
		break;
	case PlotDataset::Real:
		m_xyCurve->setRawSamples(m_dataX, m_dataY, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataY, m_dataSize);
		break;
	case PlotDataset::Imaginary:
		m_xyCurve->setRawSamples(m_dataX, m_dataYim, m_dataSize);
		m_polarData->replaceData(m_dataX, m_dataYim, m_dataSize);
		break;
	case PlotDataset::Complex:
		m_dataYcalc = new double[m_dataSize];
		m_dataXcalc = new double[m_dataSize];

		for (int i = 0; i < m_dataSize; i++) {
			m_dataXcalc[i] = atan2(m_dataYim[i], m_dataY[i]) / c_pi * 180;			// Phase for X
			double v = m_dataY[i] * m_dataY[i] + m_dataYim[i] * m_dataYim[i];	// Magnitude for Y
			v = sqrt(v);
			m_dataYcalc[i] = v;
		}
		m_xyCurve->setRawSamples(m_dataXcalc, m_dataYcalc, m_dataSize);
		m_polarData->replaceData(m_dataXcalc, m_dataYcalc, m_dataSize);
		break;
	default:
		break;
	}

	
}

// ###########################################################################

// Getter

bool PlotDataset::getDataAt(int _index, double & _x, double & _y) {
	if (_index < 0 || _index >= m_dataSize) {
		assert(0);
		return false;
	}
	_x = m_dataX[_index];
	_y = m_dataY[_index];
	return true;
}

bool PlotDataset::getData(double *& _x, double *& _y, long & _size) {
	_x = m_dataX;
	_y = m_dataY;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool PlotDataset::getYim(double *& _yim, long & _size) {
	_yim = m_dataYim;
	_size = m_dataSize;
	return m_dataSize > 0;
}

bool PlotDataset::getCopyOfData(double *& _x, double *& _y, long & _size) {
	_size = m_dataSize;
	if (m_dataSize == 0) {
		return false;
	}

	_x = new double[m_dataSize];
	_y = new double[m_dataSize];

	for (int i = 0; i < m_dataSize; i++) {
		_x[i] = m_dataX[i];
		_y[i] = m_dataY[i];
	}
	return true;
}

bool PlotDataset::getCopyOfYim(double *& _yim, long & _size) {
	_size = m_dataSize;
	if (m_dataSize == 0) {
		return false;
	}
	_yim = new double[m_dataSize];

	for (int i = 0; i < m_dataSize; i++) {
		_yim[i] = m_dataYim[i];
	}
	return true;
}

// ###########################################################################

// Private functions

void PlotDataset::memFree(void) {
	if (m_dataX != nullptr) {
		delete[] m_dataX; m_dataX = nullptr;
	}
	if (m_dataY != nullptr) {
		delete[] m_dataY; m_dataY = nullptr;
	}
	if (m_dataXcalc != nullptr) {
		delete[] m_dataXcalc; m_dataXcalc = nullptr;
	}
	if (m_dataYcalc != nullptr) {
		delete[] m_dataYcalc; m_dataYcalc = nullptr;
	}
}

void PlotDataset::repaint(void) {

	m_xyCurve->setVisible(m_isVisible);
	m_polarCurve->setVisible(m_isVisible);

	// Setup curve
	if (m_isVisible) {
		if (m_isDimmed) {
			m_xyCurve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine);
			m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine));
		}
		else {
			m_xyCurve->setPen(m_curveColor, m_curvePenSize, Qt::SolidLine);
			m_polarCurve->setPen(QPen(m_curveColor, m_curvePenSize, Qt::SolidLine));
		}
	}
	else {
		m_xyCurve->setPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen);
		m_polarCurve->setPen(QPen(QColor(100, 100, 100, 100), 0.0, Qt::NoPen));
	}

	// Setup points
	if (m_curvePointsVisible) {

		if (m_isDimmed) {
			m_xyCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_xyCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));

			m_polarCurvePointSymbol->setPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(QColor(100, 100, 100, 100));
		}
		else {
			m_xyCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_xyCurvePointSymbol->setBrush(m_curvePointInnerColor);

			m_polarCurvePointSymbol->setPen(m_curvePointOutterColor, m_curvePointOutterColorWidth, Qt::SolidLine);
			m_polarCurvePointSymbol->setBrush(m_curvePointInnerColor);
		}

		m_xyCurvePointSymbol->setSize(m_curvePointSize);
		m_polarCurvePointSymbol->setSize(m_curvePointSize);

		m_xyCurve->setSymbol(m_xyCurvePointSymbol);
	}
	else {
		m_xyCurve->setSymbol(nullptr);
	}
}

// ########################################################################################

// ########################################################################################

// ########################################################################################

PolarPlotData::PolarPlotData(double * _azimuth, double * _radius, size_t _dataSize)
	: m_azimuth(_azimuth), m_radius(_radius), m_size(_dataSize)
{

}

PolarPlotData::~PolarPlotData() {

}

QwtPointPolar PolarPlotData::sample(size_t _i) const {
	if (_i < m_size) { return QwtPointPolar(m_azimuth[_i], m_radius[_i]); }
	else { assert(0); return QwtPointPolar(0, 0); }
}

void PolarPlotData::replaceData(double * _azimuth, double * _radius, size_t _dataSize) {
	m_azimuth = _azimuth;
	m_radius = _radius;
	m_size = _dataSize;
}