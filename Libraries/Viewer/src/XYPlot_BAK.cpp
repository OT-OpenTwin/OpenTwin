#include "stdafx.h"

/*

// Open twin header
#include "XYPlot.h"
#include "DataBase.h"
#include "Viewer.h"

// QWT header
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_painter.h>
#include <qwt_scale_widget.h>

// QT header
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qevent.h>
#include <qapplication.h>

// C++ header
#include <limits>

class xyPlot::XYPlotAxisConfig {
public:
	XYPlotAxisConfig(xyPlotWidget * _plot, int _id)
		: isAutoScale(true), minScale(0.0), maxScale(0.0), axisID(_id), ownerPlot(_plot), isLogScale(false), m_logEngineSet(false),
		logScaleEngine(nullptr)
	{
		logScaleEngine = nullptr;
		linearScaleEngine = new QwtLinearScaleEngine();
		ownerPlot->setAxisScaleEngine(axisID, linearScaleEngine);
	}

	void setupAxis(void) {
		if (isLogScale && !m_logEngineSet) {
			m_logEngineSet = true;
			logScaleEngine = new QwtLogScaleEngine();
			ownerPlot->setAxisScaleEngine(axisID, logScaleEngine);
			linearScaleEngine = nullptr;
		}
		else if (!isLogScale && m_logEngineSet) {
			m_logEngineSet = false;
			linearScaleEngine = new QwtLinearScaleEngine();
			ownerPlot->setAxisScaleEngine(axisID, linearScaleEngine);
			logScaleEngine = nullptr;
		}
		ownerPlot->setAxisAutoScale(axisID, isAutoScale);
		if (!isAutoScale) {
			ownerPlot->setAxisScale(axisID, minScale, maxScale);
		}
	}

	QwtLinearScaleEngine *	linearScaleEngine;
	QwtLogScaleEngine *		logScaleEngine;
	xyPlotWidget *			ownerPlot;
	bool					isLogScale;
	bool					isAutoScale;
	double					minScale;
	double					maxScale;
	int						axisID;

private:
	bool					m_logEngineSet;
};

xyPlot::xyPlot(const QString & _title)
{
	setupWidget(_title);
}

xyPlot::xyPlot(
	const QString &			_xAxisTitle,
	const QString &			_yAxisTitle,
	const QString &			_title
) {
	setupWidget(_title);

	// Setup axis
	m_plot->setAxisTitle(QwtPlot::xBottom, _xAxisTitle);
	m_plot->setAxisTitle(QwtPlot::yLeft, _yAxisTitle);
}

xyPlot::xyPlot(
	const QString &			_xAxisBottomTitle,
	const QString &			_xAxisTopTitle,
	const QString &			_yAxisLeftTitle,
	const QString &			_yAxisRightTitle,
	const QString &			_title
) {
	setupWidget(_title);

	// Setup axis
	m_plot->setAxisTitle(QwtPlot::xBottom, _xAxisBottomTitle);
	m_plot->setAxisTitle(QwtPlot::xTop, _xAxisTopTitle);
	m_plot->setAxisTitle(QwtPlot::yLeft, _yAxisLeftTitle);
	m_plot->setAxisTitle(QwtPlot::yRight, _yAxisRightTitle);
}

xyPlot::~xyPlot() {
	for (auto itm : m_cache) {
		delete itm.second.second;
	}
	delete m_plot;
	delete m_centralLayout;
	delete m_centralWidget;
}

// #####################################################################################################

// Base class mandatory functions

void xyPlot::SetStyle(
	const QString &				_centralWidgetStylesheet,
	const QString &				_plotStyleSheet,
	const QString &				_canvasStyleSheet,
	const QPen &				_zoomerPen
) {
	m_centralWidget->setStyleSheet(_centralWidgetStylesheet);
	m_plot->SetStyleSheets(_plotStyleSheet, _canvasStyleSheet);
	m_plot->setZoomerPen(_zoomerPen);
}

// #####################################################################################################

// Getter

XYPlotDataset * xyPlot::getDataset(int _datasetId) {
	auto itm = m_datasets.find(_datasetId);
	if (itm == m_datasets.end()) {
		assert(0);		// Invalid dataset ID
		return nullptr;
	}
	else { return itm->second; }
}

PlotMarker * xyPlot::getMarker(int _markerId) {
	auto itm = m_markers.find(_markerId);
	if (itm == m_markers.end()) {
		assert(0);		// Invalid dataset ID
		return nullptr;
	}
	else { return itm->second; }
}

// #####################################################################################################

// Data manipulation

void xyPlot::setIncompatibleData(void)
{
	clear();
	setErrorState(true, "Incompatible Data");
	m_plot->replot();
}

void xyPlot::clear(void) {
	for (auto itm : m_datasets) {
		itm.second->detach();
	}
	m_plot->setTitle("");
	m_plot->setAxisTitle(xBottom, "");
	m_plot->setAxisTitle(yLeft, "");
	m_xBottom->isAutoScale = false;
	m_xBottom->isLogScale = false;
	m_xBottom->minScale = 0.0;
	m_xBottom->maxScale = 1.0;
	m_yLeft->isAutoScale = false;
	m_yLeft->isLogScale = false;
	m_yLeft->minScale = 0.0;
	m_yLeft->maxScale = 1.0;
	m_xBottom->setupAxis();
	m_yLeft->setupAxis();
	m_plot->replot();
}

void xyPlot::setFromDataBase(
	const std::string & _title,
	bool grid,
	int gridColor[],
	bool legend,
	bool logscaleX,
	bool logscaleY,
	bool autoscaleX,
	bool autoscaleY,
	double xmin,
	double xmax,
	double ymin,
	double ymax,
	const std::string & _projectName,
	const std::list<XYPlotCurveItem> &	_entityIDandVersions
) {
	// Detach all current data and refresh the title
	clear();
	m_plot->setTitle(_title.c_str());

	// Check cache
	std::list<XYPlotCurveItem> entitiesToImport;
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
	__setFromDataBase(_projectName, entitiesToImport);

	// Ensure that the axis titles are compatible
	bool compatible = true;

	std::string axisTitleX;
	std::string axisTitleY;

	if (!m_datasets.empty())
	{
		axisTitleX = m_datasets.begin()->second->getAxisTitleX();
		axisTitleY = m_datasets.begin()->second->getAxisTitleY();

		for (auto itm : m_datasets) {
			if (axisTitleX != m_datasets.begin()->second->getAxisTitleX()) compatible = false;
			if (axisTitleY != m_datasets.begin()->second->getAxisTitleY()) compatible = false;
			if (!compatible) break;
		}
	}

	if (!compatible)
	{
		setIncompatibleData();
		return;
	}

	// Setup plot
	m_plot->grid()->setVisible(grid);
	m_plot->grid()->setColor(QColor(gridColor[0], gridColor[1], gridColor[2]), false);
	m_plot->grid()->setWidth(0.5, true);

	setLegendVisible(legend);

	setAxisTitle(xBottom, axisTitleX.c_str());
	setAxisTitle(yLeft, axisTitleY.c_str());

	// Setup axis
	m_xBottom->isAutoScale = autoscaleX;
	m_xBottom->minScale = xmin;
	m_xBottom->maxScale = xmax;
	m_xBottom->isLogScale = logscaleX;

	m_yLeft->isAutoScale = autoscaleY;
	m_yLeft->minScale = ymin;
	m_yLeft->maxScale = ymax;
	m_yLeft->isLogScale = logscaleY;

	finishAddingData();
}

void xyPlot::removeFromCache(unsigned long long entityID)
{
	auto itm = m_cache.find(entityID);

	if (itm != m_cache.end())
	{
		itm->second.second->detach();
		m_datasets.erase(itm->second.second->id());
		delete itm->second.second;
		m_cache.erase(entityID);
	}
}

XYPlotDataset * xyPlot::addDataset(
	double *										_dataX,
	double *										_dataY,
	long											_dataSize
) {
	XYPlotDataset * nData = new XYPlotDataset(this, ++m_currentDataID, _dataX, _dataY, _dataSize);

	m_datasets.insert_or_assign(nData->id(), nData);
	m_curveToDatasetMap.insert_or_assign(nData->curve(), nData);
	return nData;
}

PlotMarker * xyPlot::addMarker(
	double								_x,
	double								_y,
	const QString						_text
) {
	PlotMarker * nData = new PlotMarker(++m_currentMarkerID);
	nData->setValue(QPointF(_x, _y));
	nData->setTitle(_text);
	nData->setLabel(QwtText(_text));
	nData->attach(m_plot);

	m_markers.insert_or_assign(nData->id(), nData);
	return nData;
}

void xyPlot::removeDataset(int _datasetId) {
	auto itm = m_datasets.find(_datasetId);
	if (itm == m_datasets.end()) {
		assert(0);		// Invalid dataset ID
		return;
	}
	m_curveToDatasetMap.erase(itm->second->curve());
	delete itm->second;
	m_datasets.erase(_datasetId);
}

void xyPlot::setAxisTitle(AxisID _axisID, const QString & _title) {
	m_plot->setAxisTitle(_axisID, _title);
}

void xyPlot::setAxisAutoScale(AxisID _axisID, bool _isAutoScale) {
	switch (_axisID)
	{
	case xyPlot::yLeft: m_yLeft->isAutoScale = _isAutoScale;
		break;
	case xyPlot::yRight: assert(0);
		break;
	case xyPlot::xBottom: m_xBottom->isAutoScale = _isAutoScale;
		break;
	case xyPlot::xTop: assert(0);
		break;
	default:
		assert(0);
		break;
	}
}

void xyPlot::setAxisScale(AxisID _axisID, double _min, double _max) {
	switch (_axisID)
	{
	case xyPlot::yLeft: m_yLeft->minScale = _min; m_yLeft->maxScale = _max;
		break;
	case xyPlot::yRight: assert(0);
		break;
	case xyPlot::xBottom: m_xBottom->minScale = _min; m_xBottom->maxScale = _max;
		break;
	case xyPlot::xTop: assert(0);
		break;
	default:
		assert(0);
		break;
	}
}

void xyPlot::finishAddingData(void) {
	m_xBottom->setupAxis();
	m_yLeft->setupAxis();

	m_plot->replot();

	m_plot->setZoomBase();
}

void xyPlot::resetView(void)
{
	assert(0);
}

XYPlotGrid * xyPlot::grid(void) const { return m_plot->grid(); }

void xyPlot::setLegendVisible(bool _isVisible) {
	if (_isVisible) {
		m_plot->addLegend();
	}
	else {
		m_plot->removeLegend();
	}
}

void xyPlot::datasetSelectionChanged(XYPlotDataset * _dataset) 
{
	if (m_viewer == nullptr) return;

	bool ctrlPressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);

	if (_dataset == nullptr)
	{
		if (!ctrlPressed)
		{
			m_viewer->reset1DPlotItemSelection();
		}
	}
	else
	{
		m_viewer->set1DPlotItemSelected(_dataset->getTreeItemID(), ctrlPressed);
	}
}

XYPlotDataset * xyPlot::findDataset(QwtPlotCurve * _curve) {
	auto itm = m_curveToDatasetMap.find(_curve);
	if (itm == m_curveToDatasetMap.end()) {
		return nullptr;
	}
	else {
		return itm->second;
	}
}

bool xyPlot::isControlPressed(void) const {
	return m_plot->isControlPressed();
}

void xyPlot::setTitle(const QString & _title) {

}

// #####################################################################################################

// Private functions

void xyPlot::setupWidget(const QString & _title) {
	// Create layout
	m_centralWidget = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralWidget);

	// Create controls
	m_plot = new xyPlotWidget(this, _title);
	m_errorLabel = new QLabel("Error");
	auto f = m_errorLabel->font();
	f.setPointSize(24);
	f.setBold(true);
	m_errorLabel->setFont(f);

	// Create axis data
	m_xBottom = new XYPlotAxisConfig(m_plot, QwtPlot::xBottom);
	m_yLeft = new XYPlotAxisConfig(m_plot, QwtPlot::yLeft);
	m_plot->enableAxis(QwtPlot::yRight, false);
	m_plot->enableAxis(QwtPlot::xTop, false);

	// Add controls to layout
	m_centralLayout->addWidget(m_plot);

	// Initialize data
	m_currentDataID = -1;
	m_currentMarkerID = -1;
	m_isError = false;

}

void xyPlot::__setFromDataBase(
	const std::string & _projectName,
	const std::list<XYPlotCurveItem> &	_entityIDandVersions
) {
	if (_entityIDandVersions.empty()) return;

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchCurves;
	for (auto item : _entityIDandVersions)
	{
		prefetchCurves.push_back(std::pair<unsigned long long, unsigned long long>(item.getModelEntityID(), item.getModelEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(_projectName, prefetchCurves);

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchCurveData;
	std::list< XYPlotDataset *> curves;

	for (auto item : _entityIDandVersions)
	{
		unsigned long long entityID      = item.getModelEntityID();
		unsigned long long entityVersion = item.getModelEntityVersion();

		// Read the curve data item
		auto doc = bsoncxx::builder::basic::document{};

		if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_projectName, entityID, entityVersion, doc))
		{
			assert(0);
			return;
		}

		auto doc_view = doc.view()["Found"].get_document().view();

		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

		if (entityType != "EntityResult1D")
		{
			assert(0);
			return;
		}

		int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityResult1D");
		if (schemaVersion != 1)
		{
			assert(0);
			return;
		}

		unsigned long long curveDataStorageId = doc_view["CurveDataID"].get_int64();
		unsigned long long curveDataStorageVersion = doc_view["CurveDataVersion"].get_int64();

		std::string curveLabel = item.getName();

		int color[3];
		color[0] = (int) (doc_view["colorR"].get_double() * 255.0 + 0.5);
		color[1] = (int) (doc_view["colorG"].get_double() * 255.0 + 0.5);
		color[2] = (int) (doc_view["colorB"].get_double() * 255.0 + 0.5);

		std::string xAxisLabel = doc_view["xAxisLabel"].get_utf8().value.data();
		std::string yAxisLabel = doc_view["yAxisLabel"].get_utf8().value.data();

		std::string xAxisUnit = doc_view["xAxisUnit"].get_utf8().value.data();
		std::string yAxisUnit = doc_view["yAxisUnit"].get_utf8().value.data();

		std::string axisTitleX = xAxisLabel + " [" + xAxisUnit + "]";
		std::string axisTitleY = yAxisLabel + " [" + yAxisUnit + "]";

		XYPlotDataset * newDataset = addDataset(nullptr, nullptr, 0);

		newDataset->setEntityID(entityID);
		newDataset->setEntityVersion(entityVersion);
		newDataset->setCurveEntityID(curveDataStorageId);
		newDataset->setCurveEntityVersion(curveDataStorageVersion);
		newDataset->setTreeItemID(item.getTreeID());
		newDataset->setDimmed(item.isDimmed());

		newDataset->setCurvePointsVisible(false);
		newDataset->setCurveColor(QColor(color[0], color[1], color[2]));
		newDataset->setCurveTitle(curveLabel.c_str());
		newDataset->setAxisTitleX(axisTitleX);
		newDataset->setAxisTitleY(axisTitleY);

		// Check whether we already have the curve data

		bool loadCurveDataRequired = true;

		if (m_cache.count(newDataset->getEntityID()) > 0)
		{
			XYPlotDataset * oldDataset = m_cache[newDataset->getEntityID()].second;

			if (oldDataset != nullptr)
			{
				if (   oldDataset->getCurveEntityID() == newDataset->getCurveEntityID()
					&& oldDataset->getCurveEntityVersion() == newDataset->getCurveEntityVersion())
				{
					// The curve data in the previous data set is the same as the one in the new data set

					double *x = nullptr, *y = nullptr;
					long n;

					if (oldDataset->getCopyOfData(x, y, n))
					{
						newDataset->setData(x, y, n);
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

		m_cache.insert_or_assign(newDataset->getEntityID(), std::pair<unsigned long long, XYPlotDataset *>(newDataset->getEntityVersion(), newDataset));
	}

	if (prefetchCurveData.empty()) return; // Nothing to load

	// Now load the curve data
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(_projectName, prefetchCurveData);

	for (auto item : curves)
	{
		// Here we get the storage data of the underlying curve data
		unsigned long long entityID = item->getCurveEntityID();
		unsigned long long entityVersion = item->getCurveEntityVersion();

		auto doc = bsoncxx::builder::basic::document{};

		if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_projectName, entityID, entityVersion, doc))
		{
			assert(0);
			return;
		}

		auto doc_view = doc.view()["Found"].get_document().view();

		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

		if (entityType != "EntityResult1DData")
		{
			assert(0);
			return;
		}

		int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityResult1DData");
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

		if (nYim > 0)
		{
			yim = new double[nYim];

			auto iYim = arrayYim.begin();

			for (unsigned long index = 0; index < nYim; index++)
			{
				yim[index] = iYim->get_double();
				iYim++;
			}
		}

		assert(nYim == 0); // Complex data not implemented in the viewer yet

		assert(nX == nYre);
		item->setData(x, yre, nX);
		item->attach();
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotDataset::XYPlotDataset(
	XYPlot *										_creator,
	int												_id,
	double *										_dataX,
	double *										_dataY,
	long											_dataSize
) : m_owner(_creator), m_id(_id), m_curve(nullptr), m_dataX(_dataX), m_dataY(_dataY), m_dataSize(_dataSize), m_curvePointSymbol(nullptr),
	m_curveColor(50, 50, 255), m_curveLineType(SolidLine), m_curveVisible(true), m_curvePenSize(2.0),
	m_curvePointsVisible(true), m_curvePointInnerColor(Qt::yellow), m_curvePointOutterColor(Qt::red), m_curvePointSize(8), m_curvePointOutterColorWidth(2),
	m_entityID(0), m_entityVersion(0), m_curveEntityID(0), m_curveEntityVersion(0), m_curveTreeItemID(0), m_isDimmed(false)
{
	m_curve = new QwtPlotCurve();

	m_curve->setRawSamples(m_dataX, m_dataY, m_dataSize);
	m_curve->attach(m_owner->plot());
	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

	m_curvePointSymbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::yellow), QPen(Qt::red, 2), QSize(8, 8));

	updateVisualizationSettings();
}

XYPlotDataset::~XYPlotDataset() {
	// The curve symbol must not be deleted since the curve is the owner and will clean up the memory
	if (m_dataX != nullptr) { delete[] m_dataX; }
	if (m_dataY != nullptr) { delete[] m_dataY; }
	delete m_curve;
}

// ####################################################################################

// Appearance change

void XYPlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint) {
	m_curveVisible = _isVisible;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurveWidth(double _penSize, bool _repaint) {
	m_curvePenSize = _penSize;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurveColor(const QColor & _color, bool _repaint) {
	m_curveColor = _color;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setPlotCurveLineType(PlotCurveLineType _type, bool _repaint) {
	m_curveLineType = _type;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurvePointsVisible(bool _isVisible, bool _repaint) {
	m_curvePointsVisible = _isVisible;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurvePointInnerColor(const QColor & _color, bool _repaint) {
	m_curvePointInnerColor = _color;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurvePointOuterColor(const QColor & _color, bool _repaint) {
	m_curvePointOutterColor = _color;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurvePointSize(int _size, bool _repaint) {
	m_curvePointSize = _size;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint) {
	m_curvePointOutterColorWidth = _size;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::setCurveTitle(const QString & _title) {
	m_curve->setTitle(_title);
}

void XYPlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_isDimmed = _isDimmed;
	if (_repaint) { updateVisualizationSettings(); }
}

void XYPlotDataset::attach(void) {
	m_curve->attach(m_owner->plot());
}

void XYPlotDataset::detach(void) {
	m_curve->detach();
}

void XYPlotDataset::setData(
	double *										_dataX,
	double *										_dataY,
	long											_dataSize)
{
	m_dataX = _dataX;
	m_dataY = _dataY;
	m_dataSize = _dataSize;

	m_curve->setRawSamples(_dataX, _dataY, _dataSize);
}

bool XYPlotDataset::getCopyOfData(
	double *										&_dataX,
	double *										&_dataY,
	long											&_dataSize)
{
	if (m_dataX == nullptr || m_dataY == nullptr || m_dataSize == 0) return false;

	_dataX = new double[m_dataSize];
	_dataY = new double[m_dataSize];
	_dataSize = m_dataSize;

	for (long index = 0; index < m_dataSize; index++)
	{
		_dataX[index] = m_dataX[index];
		_dataY[index] = m_dataY[index];
	}

	return true;
}

void XYPlotDataset::dataAt(
	int												_index,
	double &										_x,
	double &										_y
) {
	if (_index < 0 || _index >= m_dataSize) {
		assert(0);
		return;
	}
	_x = m_dataX[_index];
	_y = m_dataY[_index];
}

// #####################################################################################################

void XYPlotDataset::updateVisualizationSettings(void) {
	if (m_curveVisible) {
		if (m_isDimmed) {
			switch (m_curveLineType)
			{
			case XYPlotDataset::SolidLine: m_curve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::SolidLine); break;
			case XYPlotDataset::DashedLine: m_curve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::DashLine); break;
			case XYPlotDataset::DotLine: m_curve->setPen(QColor(100, 100, 100, 100), 1.0, Qt::DotLine); break;
			default:
				assert(0); // Unknown line type
				break;
			}
		}
		else {
			switch (m_curveLineType)
			{
			case XYPlotDataset::SolidLine: m_curve->setPen(m_curveColor, m_curvePenSize, Qt::SolidLine); break;
			case XYPlotDataset::DashedLine: m_curve->setPen(m_curveColor, m_curvePenSize, Qt::DashLine); break;
			case XYPlotDataset::DotLine: m_curve->setPen(m_curveColor, m_curvePenSize, Qt::DotLine); break;
			default:
				assert(0); // Unknown line type
				break;
			}
		}
	}
	else {
		m_curve->setPen(m_curveColor, m_curvePenSize, Qt::NoPen);
	}

	if (m_curvePointsVisible) {
		if (m_isDimmed) {
			m_curvePointSymbol->setBrush(QBrush(QColor(100, 100, 100, 100)));
			m_curvePointSymbol->setPen(QPen(QColor(100, 100, 100, 100), m_curvePointOutterColorWidth));
		}
		else {
			m_curvePointSymbol->setBrush(QBrush(m_curvePointInnerColor));
			m_curvePointSymbol->setPen(QPen(m_curvePointOutterColor, m_curvePointOutterColorWidth));
		}
		m_curvePointSymbol->setSize(QSize(m_curvePointSize, m_curvePointSize));
		m_curve->setSymbol(m_curvePointSymbol);
	}
	else {
		m_curve->setSymbol(nullptr);
	}

	m_curve->plot();
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

PlotMarker::PlotMarker(int _id)
	: m_id(_id)
{
	setLineStyle(QwtPlotMarker::LineStyle::Cross);
	setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	setLinePen(QPen(QColor(200, 150, 0), 0, Qt::DashDotLine));
	m_symbol = new QwtSymbol(QwtSymbol::Diamond, QColor(Qt::yellow), QColor(Qt::green), QSize(7, 7));
	setSymbol(m_symbol);
}

PlotMarker::~PlotMarker() {

}

void PlotMarker::setStyle(const QColor & _innerColor, const QColor & _outerColor, int _size, double _outerColorSize) {
	m_symbol->setBrush(_innerColor);
	m_symbol->setPen(QPen(_outerColor, _outerColorSize));
	m_symbol->setSize(QSize(_size, _size));
}

void PlotMarker::SetLineStyle(const QString & _style) {
	if (_style == "Cross") { setLineStyle(Cross); }
	else if (_style == "HLine") { setLineStyle(HLine); }
	else if (_style == "VLine") { setLineStyle(VLine); }
	else { setLineStyle(NoLine); }
}

QString PlotMarker::lineStyleToString(void) const {
	switch (lineStyle())
	{
	case Cross: return "Cross";
	case HLine: return "HLine";
	case VLine: return "VLine";
	case NoLine: return "NoLine";
	default:
		assert(0);
		return "NoLine";
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotWidget::xyPlotWidget(xyPlot * _owner, const QString & _plotTitle)
	: m_owner(_owner)
{
	m_canvas = new xyPlotWidgetCanvas(this);
	setCanvas(m_canvas);

	m_legend = new XYPlotLegend(this);

	m_plotPanner = new XYPlotPanner(canvas());
	m_plotPanner->setMouseButton(Qt::MouseButton::MiddleButton);

	m_plotZoomer = new XYPlotZoomer(this, canvas());

	m_plotMagnifier = new XYPlotMagnifier(this, canvas());
	
	m_grid = new XYPlotGrid();
	m_grid->attach(this);

	m_picker = new XYPlotPicker(canvas());
}

xyPlotWidget::~xyPlotWidget() {

}

void xyPlotWidget::SetStyleSheets(
	const QString &				_plotStyleSheet,
	const QString &				_canvasStyleSheet
) {
	m_canvas->setStyleSheet(_canvasStyleSheet);
	setStyleSheet(_plotStyleSheet);
}

void xyPlotWidget::setZoomerPen(
	const QPen &				_pen
) {
	m_plotZoomer->setRubberBandPen(_pen);
	m_plotZoomer->setTrackerPen(_pen);
}

void xyPlotWidget::setZoomBase() {
	m_plotZoomer->setZoomBase();
}

QwtPlotCurve * xyPlotWidget::findNearestCurve(const QPoint & _pos, int & _pointID) {
	double dist{ DBL_MAX };

	QwtPlotCurve * curve = nullptr;

	for (QwtPlotItemIterator it = itemList().begin();
		it != itemList().end(); ++it)
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
				_pointID = idx;
			}
		}
	}

	return curve;
}

void xyPlotWidget::addLegend(void) {
	if (m_legend == nullptr) {
		m_legend = new XYPlotLegend(this);
	}
}

void xyPlotWidget::removeLegend(void) {
	if (m_legend != nullptr) {
		delete m_legend;
		m_legend = nullptr;
	}
}

bool xyPlotWidget::isControlPressed(void) const {
	return m_plotZoomer->isControlPressed();
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

xyPlotWidgetCanvas::xyPlotWidgetCanvas(xyPlotWidget * _plot)
	: m_plot(_plot) {}

xyPlotWidgetCanvas::~xyPlotWidgetCanvas() {

}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotPanner::XYPlotPanner(QWidget * _parent) 
	: QwtPlotPanner(_parent), m_mouseIsPressed(false)
{
	setMouseButton(Qt::MouseButton::MiddleButton);
}

bool XYPlotPanner::eventFilter(QObject * _object, QEvent * _event) {
	if (_object == nullptr || _object != parentWidget()) { return false; }

	switch (_event->type())
	{
	case QEvent::MouseButtonPress:
	{
		auto evr = static_cast<QMouseEvent *>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = true;
			widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseMove:
	{
		QMouseEvent * evr = static_cast<QMouseEvent *>(_event);
		if (m_mouseIsPressed) {
			widgetMouseMoveEvent(evr);
			widgetMouseReleaseEvent(evr);
			setMouseButton(evr->button(), evr->modifiers());
			widgetMousePressEvent(evr);
		}
		break;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent * evr = static_cast<QMouseEvent *>(_event);
		if (evr->button() == Qt::MouseButton::MiddleButton) {
			m_mouseIsPressed = false;
			widgetMouseReleaseEvent(evr);
		}
		break;
	}
	case QEvent::KeyPress:
	{
		//widgetKeyPressEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::KeyRelease:
	{
		//widgetKeyReleaseEvent(static_cast<QKeyEvent *>(_event));
		break;
	}
	case QEvent::Paint:
	{
		if (isVisible())
			return true;
		break;
	}
	default:;
	}

	return false;
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotZoomer::XYPlotZoomer(xyPlotWidget * _plot, QWidget * _parent)
	: QwtPlotZoomer(_parent), m_plot(_plot), m_mouseMoved(false), m_controlIsPressed(false)
{}

void XYPlotZoomer::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		m_mouseMoved = false;
		QwtPlotZoomer::widgetMousePressEvent(_event);
	}
}

void XYPlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseDoubleClickEvent(_event);
	}
	int pointId;
	m_plot->Owner()->datasetSelectionChanged(m_plot->Owner()->findDataset(m_plot->findNearestCurve(_event->pos(), pointId)));
}

void XYPlotZoomer::widgetMouseMoveEvent(QMouseEvent * _event) {
	m_mouseMoved = true;
	QwtPlotZoomer::widgetMouseMoveEvent(_event);
}

void XYPlotZoomer::widgetWheelEvent(QWheelEvent * _event) {
		
}

void XYPlotZoomer::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::LeftButton) {
		QwtPlotZoomer::widgetMouseReleaseEvent(_event);
	}
	else if (_event->button() == Qt::MouseButton::RightButton) {
		if (!m_mouseMoved) {
			QwtPlotZoomer::widgetMouseReleaseEvent(_event);
		}
	}
}

void XYPlotZoomer::widgetKeyPressEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = true;
	}
}

void XYPlotZoomer::widgetKeyReleaseEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key::Key_Control) {
		m_controlIsPressed = false;
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotMagnifier::XYPlotMagnifier(xyPlotWidget * _plot, QWidget * _parent)
	: QwtPlotMagnifier(_parent), m_rightMouseIsPressed(false), m_mouseMoved(false), m_plot(_plot)
{
	m_marker = new PlotMarker(0);
	m_marker->attach(m_plot);
	m_marker->setVisible(false);
	m_marker->setLinePen(QColor(255, 50, 50), 0.0, Qt::DashDotDotLine);

	setMouseButton(Qt::MouseButton::NoButton);
}

void XYPlotMagnifier::widgetMousePressEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		m_rightMouseIsPressed = true;
		m_mouseMoved = false;
	}
	QwtPlotMagnifier::widgetMousePressEvent(_event);
}

void XYPlotMagnifier::widgetMouseMoveEvent(QMouseEvent * _event) {

	m_mouseMoved = true;

	if (m_rightMouseIsPressed) {
		int itemId;
		QwtPlotCurve * curve = m_plot->findNearestCurve(_event->pos(), itemId);

		if (curve != nullptr) {
			double x, y;
			m_plot->Owner()->findDataset(curve)->dataAt(itemId, x, y);
			m_marker->setValue(x, y);
			m_marker->setLabel(QwtText((std::to_string(x) + ", " + std::to_string(y)).c_str()));
			m_marker->setVisible(true);
			m_plot->replot();
		}
	}
	QwtPlotMagnifier::widgetMouseMoveEvent(_event);
}

void XYPlotMagnifier::widgetMouseReleaseEvent(QMouseEvent * _event) {
	if (_event->button() == Qt::MouseButton::RightButton) {
		if (m_mouseMoved) {
			m_marker->setVisible(false);
			m_plot->replot();
		}
		m_rightMouseIsPressed = false;
	}
	QwtPlotMagnifier::widgetMouseReleaseEvent(_event);
}

void XYPlotMagnifier::widgetWheelEvent(QWheelEvent * _wheelEvent) {
	m_cursorPos = _wheelEvent->pos();
	
	if (_wheelEvent->modifiers() != wheelModifiers()) { return; }

	if (wheelFactor() != 0.0)
	{
		int delta = _wheelEvent->delta() * (-1);
		double f = qPow(wheelFactor(),
			qAbs(delta / 120.0));

		if (delta > 0)
			f = 1 / f;

		rescale(f);
	}

}

void XYPlotMagnifier::rescale(double _factor) {
	QwtPlot* plt = plot();
	if (plt == nullptr)
		return;

	_factor = qAbs(_factor);
	if (_factor == 1.0 || _factor == 0.0) { return; }

	bool doReplot = false;

	const bool autoReplot = plt->autoReplot();
	plt->setAutoReplot(false);

	for (int axisId = 0; axisId < QwtPlot::axisCnt; axisId++)
	{
		if (isAxisEnabled(axisId))
		{
			const QwtScaleMap scaleMap = plt->canvasMap(axisId);

			double v1 = scaleMap.s1(); //v1 is the bottom value of the axis scale
			double v2 = scaleMap.s2(); //v2 is the top value of the axis scale

			if (scaleMap.transformation())
			{
				// the coordinate system of the paint device is always linear
				v1 = scaleMap.transform(v1); // scaleMap.p1()
				v2 = scaleMap.transform(v2); // scaleMap.p2()
			}

			double c = 0; //represent the position of the cursor in the axis coordinates
			if (axisId == QwtPlot::xBottom) //we only work with these two axis
			{
				c = scaleMap.invTransform(m_cursorPos.x());
				if (scaleMap.transformation())
				{
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}

			}
			if (axisId == QwtPlot::yLeft) {
				c = scaleMap.invTransform(m_cursorPos.y());
				if (scaleMap.transformation())
				{
					c = scaleMap.transform(c); // Revert the transformation in case that the currently used scale engine is using a log scale
				}
			}
			const double center = 0.5 * (v1 + v2);
			const double width_2 = 0.5 * (v2 - v1) * _factor;
			const double newCenter = c - _factor * (c - center);

			v1 = newCenter - width_2;
			v2 = newCenter + width_2;

			if (scaleMap.transformation())
			{
				v1 = scaleMap.invTransform(v1);
				v2 = scaleMap.invTransform(v2);
			}

			plt->setAxisScale(axisId, v1, v2);
			doReplot = true;
		}
	}

	plt->setAutoReplot(autoReplot);

	if (doReplot) {
		plt->replot();
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotLegend::XYPlotLegend(xyPlotWidget * _owner)
	: m_owner(_owner)
{
	m_owner->insertLegend(this);
}

XYPlotLegend::~XYPlotLegend() {

}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotGrid::XYPlotGrid() 
	: m_color(100, 100, 100), m_width(0.5)
{
	setPen(QPen(QBrush(m_color), m_width));
}

XYPlotGrid::~XYPlotGrid() {

}

void XYPlotGrid::setColor(const QColor & _color, bool _repaint) {
	m_color = _color;
	if (_repaint) {
		setPen(QPen(QBrush(m_color), m_width));
	}
}

void XYPlotGrid::setWidth(double _width, bool _repaint) {
	m_width = _width;
	if (_repaint) {
		setPen(QPen(QBrush(m_color), m_width));
	}
}

// #####################################################################################################

// #####################################################################################################

// #####################################################################################################

XYPlotPicker::XYPlotPicker(QWidget * _parent)
	: QwtPlotPicker(_parent)
{

}

XYPlotPicker::~XYPlotPicker() {

}

*/
