//! @file PlotDataset.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Plot1DCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>

// std header
#include <string>

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;

namespace ot {

	class Plot;
	class PolarPlotData;

	class OT_WIDGETS_API_EXPORT PlotDataset {
		OT_DECL_NOCOPY(PlotDataset)
		OT_DECL_NODEFAULT(PlotDataset)
	public:
		PlotDataset(Plot* _ownerPlot, int _id, const QString& _title, double* _dataX, double* _dataY, long _dataSize);
		virtual ~PlotDataset();

		// ###########################################################################

		// Setter

		void replaceData(double* _dataX, double* _dataY, long _dataSize);

		void setYim(double* _dataYim) { m_dataYim = _dataYim; }

		void setCurveIsVisibile(bool _isVisible = true, bool _repaint = true);

		void setCurveWidth(double _penSize, bool _repaint = true);

		void setCurveColor(const QColor& _color, bool _repaint = true);

		void setCurvePointsVisible(bool _isVisible = true, bool _repaint = true);

		void setCurvePointInnerColor(const QColor& _color, bool _repaint = true);

		void setCurvePointOuterColor(const QColor& _color, bool _repaint = true);

		void setCurvePointSize(int _size, bool _repaint = true);

		void setCurvePointOuterColorWidth(double _size, bool _repaint = true);

		void attach(void);

		void detach(void);

		void setCurveTitle(const QString& _title);
		const QString& getCurveTitle(void) const { return m_curveTitle; };

		void setDimmed(bool _isDimmed, bool _repaint = true);

		void setEntityID(unsigned long long _entityID) { m_entityID = _entityID; }
		void setEntityVersion(unsigned long long _entityVersion) { m_entityVersion = _entityVersion; }
		void setCurveEntityID(unsigned long long _entityID) { m_curveEntityID = _entityID; }
		void setCurveEntityVersion(unsigned long long _entityVersion) { m_curveEntityVersion = _entityVersion; }
		void setTreeItemID(unsigned long long _treeItemID) { m_curveTreeItemID = _treeItemID; }
		void setAxisTitleX(const std::string& _title) { m_axisTitleX = _title; }
		void setAxisTitleY(const std::string& _title) { m_axisTitleY = _title; }

		void calculateData(Plot1DCfg::AxisQuantity _axisQuantity);

		// ###########################################################################

		// Getter

		bool getCurveIsVisible(void) const { return m_isVisible; };

		int id(void) const { return m_id; }

		bool getDataAt(int _index, double& _x, double& _y);

		bool getData(double*& _x, double*& _y, long& _size);

		bool getYim(double*& _yim, long& _size);

		bool getCopyOfData(double*& _x, double*& _y, long& _size);

		bool getCopyOfYim(double*& _yim, long& _size);

		unsigned long long getEntityID(void) { return m_entityID; }
		unsigned long long getEntityVersion(void) { return m_entityVersion; }
		unsigned long long getCurveEntityID(void) { return m_curveEntityID; }
		unsigned long long getCurveEntityVersion(void) { return m_curveEntityVersion; }
		unsigned long long getTreeItemID(void) { return m_curveTreeItemID; }
		std::string getAxisTitleX(void) { return m_axisTitleX; }
		std::string getAxisTitleY(void) { return m_axisTitleY; }

		void updateVisualization(void);

	private:

		void memFree(void);

		friend class Plot;

		Plot* m_ownerPlot;

		int							m_id;
		PolarPlotData* m_polarData;
		double* m_dataX;
		double* m_dataXcalc;
		double* m_dataY;
		double* m_dataYim;
		double* m_dataYcalc;
		long						m_dataSize;

		bool						m_isAttatched;
		bool						m_isVisible;
		bool						m_isDimmed;

		QString						m_curveTitle;

		QwtPlotCurve* m_cartesianCurve;
		QwtPolarCurve* m_polarCurve;

		QwtSymbol* m_cartesianCurvePointSymbol;
		QwtSymbol* m_polarCurvePointSymbol;

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
	};

}