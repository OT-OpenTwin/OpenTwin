/*
 *	File:		Plot.h
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Wrapper header
#include <qwtw/globalDatatypes.h>
#include <qwtw/AbstractPlot.h>

// Qwt header
#include <qwt_series_data.h>
#include <qwt_point_polar.h>

// Qt header
#include <qobject.h>
#include <qstring.h>
#include <qpen.h>

// C++ header
#include <map>

class QWidget;
class QLabel;
class QVBoxLayout;

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;

namespace qwtw {
	class Plot;
	class PlotDataset;

	class XYPlot;
	class PolarPlot;

	class PolarPlotData;

	Q_DECLARE_METATYPE(PlotDataset *);

	class QWTW_LIB_EXPORT AbstractPlotDatasetInformation {
	public:
		AbstractPlotDatasetInformation() {};
		virtual ~AbstractPlotDatasetInformation() {};
	};

	// ########################################################################################

	// ########################################################################################

	// ########################################################################################

	class QWTW_LIB_EXPORT PlotDataset {
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
			XYPlot *				_xyPlot,
			PolarPlot *				_polarPlot,
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

		void setInformation(AbstractPlotDatasetInformation * _information) { m_information = _information; }
		
		void setAxisTitleX(const std::string &title) { m_axisTitleX = title; }
		void setAxisTitleY(const std::string &title) { m_axisTitleY = title; }

		void calculateData(axisQuantity _complexType);

		void repaint(void);

		// ###########################################################################

		// Getter

		int id(void) const { return m_id; }

		bool getDataAt(int _index, double & _x, double & _y);

		bool getData(double *& _x, double *& _y, long & _size);

		bool getYim(double *& _yim, long & _size);

		bool getCopyOfData(double *& _x, double *& _y, long & _size);

		bool getCopyOfYim(double *& _yim, long & _size);

		AbstractPlotDatasetInformation * information(void) { return m_information; }
		
		std::string getAxisTitleX(void) { return m_axisTitleX; }
		std::string getAxisTitleY(void) { return m_axisTitleY; }

	private:

		void memFree(void);

		friend class qwtw::Plot;

		Plot *								m_owner;
		XYPlot *							m_xyPlot;
		PolarPlot *							m_polarPlot;

		int									m_id;
		PolarPlotData *						m_polarData;
		double *							m_dataX;
		double *							m_dataXcalc;
		double *							m_dataY;
		double *							m_dataYim;
		double *							m_dataYcalc;
		long								m_dataSize;

		bool								m_isAttatched;
		bool								m_isVisible;
		bool								m_isDimmed;

		QString								m_curveTitle;

		QwtPlotCurve *						m_xyCurve;
		QwtPolarCurve *						m_polarCurve;

		QwtSymbol *							m_xyCurvePointSymbol;
		QwtSymbol *							m_polarCurvePointSymbol;

		QColor								m_curveColor;
		double								m_curvePenSize;

		bool								m_curvePointsVisible;
		QColor								m_curvePointOutterColor;
		QColor								m_curvePointInnerColor;
		double								m_curvePointOutterColorWidth;
		int									m_curvePointSize;

		AbstractPlotDatasetInformation *	m_information;

		std::string							m_axisTitleX;
		std::string							m_axisTitleY;

		PlotDataset() = delete;
		PlotDataset(const PlotDataset &) = delete;
		PlotDataset & operator = (const PlotDataset &) = delete;
	};

	// ########################################################################################

	// ########################################################################################

	// ########################################################################################

	class QWTW_LIB_EXPORT Plot : public QObject {
		Q_OBJECT
	public:
		Plot();

		virtual ~Plot();

		// ###########################################################################

		// Getter

		QWidget * widget(void) const { return m_centralWidget; }

		// ###########################################################################

		// Setter

		void setPlotType(AbstractPlot::PlotType _type);
		void setTitle(const QString& _title);

		void setGridVisible(bool _isVisible, bool _repaint = true);
		void setGridColor(const QColor& _color, bool _repaint = true);
		void setGridLineWidth(double _width, bool _repaint = true);

		void setLegendVisible(bool _isVisible, bool _repaint = true);

		void setAxisTitle(AbstractPlotAxis::AxisID _axis, const QString& _title);
		void setAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _autoScaleEnabled);
		void setAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _logScaleEnabled);
		void setAxisMinValue(AbstractPlotAxis::AxisID _axis, double _minValue);
		void setAxisMaxValue(AbstractPlotAxis::AxisID _axis, double _maxValue);

		void setAxisValueLabels(AbstractPlotAxis::AxisID _axis, const std::map<double, QString>& _valueToLabelMap);
		void clearAxisValueLabels();
		void clearAxisValueLabels(AbstractPlotAxis::AxisID _axis);

		void resetView(void);

		// ###########################################################################

		// Data handling

		PlotDataset * addDataset(
			const QString &			_title,
			double *				_dataX,
			double *				_dataY,
			long					_dataSize,
			bool					_addToCache = true
		);

		virtual void datasetSelectionChanged(PlotDataset * _selectedDataset);

		void setErrorState(bool _isError, const QString & _message = QString());

		void setIncompatibleData(void);

		void repaintAll(void);

		void refresh(void);

		void clear(bool _clearCache, bool _clearAxisSettings);

		void removeFromCache(unsigned long long _entityID);

		PlotDataset * findDataset(QwtPlotCurve * _curve);

		PlotDataset * findDataset(QwtPolarCurve * _curve);

		void setAxisQuantity(PlotDataset::axisQuantity _type);

		// ###########################################################################

		// Plot settings

		void SetStyleSheets(const std::string & _plotBackgroundSheet, const std::string & _xyPlot, const std::string & _xyCanvas, const QColor & _polarPlotColor, const QPen & _zoomerPen);

	signals:
		void selectionChanged(PlotDataset * _selectedDataset, bool _isControlPressed);

	private:

		QWidget *						m_centralWidget;
		QLabel *						m_errorLabel;
		QVBoxLayout *					m_centralLayout;

		XYPlot *						m_xyPlot;
		PolarPlot *						m_polarPlot;

		AbstractPlot::PlotType			m_currentPlot;

		bool							m_isError;

		unsigned long					m_currentDatasetId;

		std::map <unsigned long long,
			std::pair<unsigned long long,
			PlotDataset *>>				m_cache;

		Plot(Plot&) = delete;
		Plot& operator = (Plot&) = delete;
	};

	// ########################################################################################

	// ########################################################################################

	// ########################################################################################

	class QWTW_LIB_EXPORT PolarPlotData : public QwtSeriesData<QwtPointPolar> {
	public:
		PolarPlotData(double * _azimuth, double * _radius, size_t _dataSize);
		virtual ~PolarPlotData();

		virtual size_t size(void) const override { return m_size; }

		virtual QwtPointPolar sample(size_t _i) const override;

		virtual QRectF boundingRect(void) const override;

		void replaceData(double * _azimuth, double * _radius, size_t _dataSize);

	private:
		double *		m_azimuth;
		double *		m_radius;
		size_t			m_size;
	};
}