#pragma once

#include <qstring.h>
#include <qcolor.h>

#include <qwt_plot.h>
#include <qwt_polar.h>

class AbstractPlot;
class PlotDataset;

class AbstractPlotAxis {
public:
	enum AxisID {
		yLeft = 0,
		yRight = 1,
		xBottom = 2,
		xTop = 3
	};

	AbstractPlotAxis(AxisID _id);

	virtual ~AbstractPlotAxis();

	// #############################################################################

	virtual void setTitle(const QString & _title);

	virtual void setAutoScale(bool _isAutoScale);

	virtual void setLogScale(bool _isLogScale);
	
	virtual void setMin(double _minValue);

	virtual void setMax(double _maxValue);

	virtual void repaint(void) = 0;

	QwtPlot::Axis xyAxisID(void) const;

	QwtPolar::Axis polarAxisID(void) const;

protected:
	AxisID			m_id;

	QString			m_title;
	bool			m_isAutoScale;
	bool			m_isLogScale;
	bool			m_isLogScaleLast;
	double			m_minValue;
	double			m_maxValue;

private:
	AbstractPlotAxis() = delete;
	AbstractPlotAxis(AbstractPlotAxis &) = delete;
	AbstractPlotAxis& operator = (AbstractPlotAxis&) = delete;
};

// ###########################################################################################

// ###########################################################################################

// ###########################################################################################

class PlotConfiguration {
public:
	PlotConfiguration();

	bool		gridIsVisible;
	QColor		gridColor;
	double		gridLineWidth;

	bool		legendIsVisible;

};

// ###########################################################################################

// ###########################################################################################

// ###########################################################################################

class AbstractPlot {
public:

	enum PlotType {
		Cartesian,
		Polar
	};

	static std::string plotTypeString(PlotType _type);

	static PlotType plotTypeFromString(const std::string& _string, bool& _isComplex);

	AbstractPlot(PlotType _type);

	virtual ~AbstractPlot();

	// ################################################################

	// Plot

	void SetLegendVisible(bool _isVisible = true, bool _repaint = true);

	virtual void RepaintLegend(void) = 0;

	virtual void RefreshWholePlot(void) = 0;

	virtual void Clear(void) = 0;

	// ################################################################

	// Axis

	void SetAxis(AbstractPlotAxis * _axisXBottom, AbstractPlotAxis * _axisXTop, AbstractPlotAxis * _axisYLeft, AbstractPlotAxis * _axisYRight);

	AbstractPlotAxis * GetAxis(AbstractPlotAxis::AxisID _id);

	void SetAxisTitle(AbstractPlotAxis::AxisID _axis, const QString & _title);

	void SetAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale);

	void SetAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale);

	void SetAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue);

	void SetAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue);

	void RepaintAxis(AbstractPlotAxis::AxisID _axis);

	// ################################################################

	// Grid

	void SetGridVisible(bool _isVisible, bool _repaint = true);

	void SetGridColor(const QColor & _color, bool _repaint = true);

	void SetGridLineWidth(double _width, bool _repaint = true);

	virtual void RepaintGrid(void) = 0;

	// ################################################################

	// Getter

	PlotType Type(void) const { return m_type; }

protected:

	AbstractPlotAxis *		m_axisXBottom;
	AbstractPlotAxis *		m_axisXTop;
	AbstractPlotAxis *		m_axisYLeft;
	AbstractPlotAxis *		m_axisYRight;

	PlotConfiguration		m_config;

	PlotType				m_type;

private:

	AbstractPlot() = delete;
	AbstractPlot(AbstractPlot&) = delete;
	AbstractPlot& operator = (AbstractPlot&) = delete;

};

