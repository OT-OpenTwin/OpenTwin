#pragma once

#include <qstring.h>
#include <qpen.h>

#include "AbstractPlot.h"

#include <map>

#include <qwt_series_data.h>
#include <qwt_point_polar.h>

class QWidget;
class QLabel;
class QVBoxLayout;

class Viewer;
class Plot;
class xyPlot;
class polarPlot;

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;

class PolarPlotData;
class PlotDataset;

class PlotCurveItem
{
public:
	PlotCurveItem() : modelEntityID(0), modelEntityVersion(0), treeID(0), dimmed(false) {};
	virtual ~PlotCurveItem() {};

	void setName(const std::string &_name) { name = _name; }
	void setModelEntity(unsigned long long _modelEntityID, unsigned long long _modelEntityVersion) { modelEntityID = _modelEntityID;  modelEntityVersion = _modelEntityVersion; }
	void setTreeID(unsigned long long _UID) { treeID = _UID; }
	void setDimmed(bool _dimmed) { dimmed = _dimmed; }

	std::string getName(void) { return name; }
	unsigned long long getModelEntityID(void) { return modelEntityID; }
	unsigned long long getModelEntityVersion(void) { return modelEntityVersion; }
	unsigned long long getTreeID(void) { return treeID; }
	bool isDimmed(void) { return dimmed; }

private:
	std::string name;
	unsigned long long modelEntityID;
	unsigned long long modelEntityVersion;
	unsigned long long treeID;
	bool dimmed;
};

// ########################################################################################

// ########################################################################################

// ########################################################################################

class PlotDataset {
public:

	enum axisQuantity {
		Magnitude,
		Phase,
		Real,
		Imaginary,
		Complex
	};

	static std::string plotQuantityString(axisQuantity _quantity);

	static axisQuantity plotQuantityFromString(const std::string& _string);

	PlotDataset(
		Plot *					_owner,
		xyPlot *				_xyPlot,
		polarPlot *				_polarPlot,
		int						_id,
		const QString &			_title,
		double *				_dataX,
		double *				_dataY,
		long					_dataSize
	);

	virtual ~PlotDataset();

	// ###########################################################################

	// Setter

	void replaceData(double * _dataX, double * _dataY, long _dataSize);

	void setYim(double * _dataYim) { m_dataYim = _dataYim; }
	
	void setCurveIsVisibile(bool _isVisible = true, bool _repaint = true);

	void setCurveWidth(double _penSize, bool _repaint = true);

	void setCurveColor(const QColor & _color, bool _repaint = true);

	void setCurvePointsVisible(bool _isVisible = true, bool _repaint = true);

	void setCurvePointInnerColor(const QColor & _color, bool _repaint = true);

	void setCurvePointOuterColor(const QColor & _color, bool _repaint = true);

	void setCurvePointSize(int _size, bool _repaint = true);

	void setCurvePointOuterColorWidth(double _size, bool _repaint = true);

	void attach(void);

	void detach(void);

	void setCurveTitle(const QString & _title);

	void setDimmed(bool _isDimmed, bool _repaint = true);

	void setEntityID(unsigned long long entityID) { m_entityID = entityID; }
	void setEntityVersion(unsigned long long entityVersion) { m_entityVersion = entityVersion; }
	void setCurveEntityID(unsigned long long entityID) { m_curveEntityID = entityID; }
	void setCurveEntityVersion(unsigned long long entityVersion) { m_curveEntityVersion = entityVersion; }
	void setTreeItemID(unsigned long long treeItemID) { m_curveTreeItemID = treeItemID; }
	void setAxisTitleX(const std::string &title) { m_axisTitleX = title; }
	void setAxisTitleY(const std::string &title) { m_axisTitleY = title; }

	void calculateData(axisQuantity _complexType);

	// ###########################################################################

	// Getter

	int id(void) const { return m_id; }

	bool getDataAt(int _index, double & _x, double & _y);

	bool getData(double *& _x, double *& _y, long & _size);

	bool getYim(double *& _yim, long & _size);

	bool getCopyOfData(double *& _x, double *& _y, long & _size);

	bool getCopyOfYim(double *& _yim, long & _size);

	unsigned long long getEntityID(void) { return m_entityID; }
	unsigned long long getEntityVersion(void) { return m_entityVersion; }
	unsigned long long getCurveEntityID(void) { return m_curveEntityID; }
	unsigned long long getCurveEntityVersion(void) { return m_curveEntityVersion; }
	unsigned long long getTreeItemID(void) { return m_curveTreeItemID; }
	std::string getAxisTitleX(void) { return m_axisTitleX; }
	std::string getAxisTitleY(void) { return m_axisTitleY; }

private:

	void memFree(void);

	void repaint(void);

	friend class Plot;

	Plot *						m_owner;
	xyPlot *					m_xyPlot;
	polarPlot *					m_polarPlot;

	int							m_id;
	PolarPlotData *				m_polarData;
	double *					m_dataX;
	double *					m_dataXcalc;
	double *					m_dataY;
	double *					m_dataYim;
	double *					m_dataYcalc;
	long						m_dataSize;

	bool						m_isAttatched;
	bool						m_isVisible;
	bool						m_isDimmed;

	QString						m_curveTitle;

	QwtPlotCurve *				m_xyCurve;
	QwtPolarCurve *				m_polarCurve;

	QwtSymbol *					m_xyCurvePointSymbol;
	QwtSymbol *					m_polarCurvePointSymbol;

	QColor						m_curveColor;
	double						m_curvePenSize;

	bool						m_curvePointsVisible;
	QColor						m_curvePointOutterColor;
	QColor						m_curvePointInnerColor;
	double						m_curvePointOutterColorWidth;
	int							m_curvePointSize;

	unsigned long long			m_entityID;
	unsigned long long			m_entityVersion;
	unsigned long long			m_curveEntityID;
	unsigned long long			m_curveEntityVersion;
	unsigned long long			m_curveTreeItemID;

	std::string					m_axisTitleX;
	std::string					m_axisTitleY;

	PlotDataset() = delete;
	PlotDataset(const PlotDataset &) = delete;
	PlotDataset & operator = (const PlotDataset &) = delete;
};

// ########################################################################################

// ########################################################################################

// ########################################################################################

class Plot {
public:

	Plot(Viewer * _viewer = (Viewer*)nullptr);

	virtual ~Plot();

	// ###########################################################################

	// Getter

	QWidget * widget(void) const { return m_centralWidget; }

	// ###########################################################################

	// Setter

	void setViewer(Viewer* _viewer) { m_viewer = _viewer; };

	void setPlotType(AbstractPlot::PlotType _type);

	PlotDataset * addDataset(
		const QString &			_title,
		double *				_dataX,
		double *				_dataY,
		long					_dataSize
	);

	//! @brief Will clear the plot and import the data from the database
	//! @param _title The title of the plot
	//! @param _projectName The name of the project
	//! @param _entityIDandVersions A list containing entityID-entityVersion pairs
	void setFromDataBase(
		AbstractPlot::PlotType				_type,
		PlotDataset::axisQuantity			_yAxisType,
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
	);

	void resetView(void);

	// ###########################################################################

	// Data handling

	void setErrorState(bool _isError, const QString & _message = QString());

	void setIncompatibleData(void);

	void refresh(void);

	void clear(bool _clearCache);

	void removeFromCache(unsigned long long _entityID);

	void datasetSelectionChanged(PlotDataset * _selectedDataset);

	PlotDataset * findDataset(QwtPlotCurve * _curve);

	PlotDataset * findDataset(QwtPolarCurve * _curve);

	void setAxisQuantity(PlotDataset::axisQuantity _type);

private:

	void setFromDataBase(const std::string & _projectName, const std::list<PlotCurveItem> & _entitiesToImport);

	Viewer *						m_viewer;

	QWidget *						m_centralWidget;
	QLabel *						m_errorLabel;
	QVBoxLayout *					m_centralLayout;

	xyPlot *						m_xyPlot;
	polarPlot *						m_polarPlot;

	AbstractPlot::PlotType			m_currentPlot;

	bool							m_isError;

	unsigned long					m_currentDatasetId;
	
	std::map <unsigned long long,
		std::pair<unsigned long long,
		PlotDataset *>>				m_cache;

};

// ########################################################################################

// ########################################################################################

// ########################################################################################

class PolarPlotData : public QwtSeriesData<QwtPointPolar> {
public:
	PolarPlotData(double * _azimuth, double * _radius, size_t _dataSize);
	virtual ~PolarPlotData();

	virtual size_t size(void) const override { return m_size; }

	virtual QwtPointPolar sample(size_t _i) const override;

	void replaceData(double * _azimuth, double * _radius, size_t _dataSize);

private:
	double *		m_azimuth;
	double *		m_radius;
	size_t			m_size;
};
