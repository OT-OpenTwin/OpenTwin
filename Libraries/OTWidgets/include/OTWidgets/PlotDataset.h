//! @file PlotDataset.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTWidgets/PlotDatasetData.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>

// std header
#include <string>

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;

namespace ot {

	class PlotBase;
	class PolarPlotData;

	class OT_WIDGETS_API_EXPORT PlotDataset {
		OT_DECL_NOCOPY(PlotDataset)
		OT_DECL_NODEFAULT(PlotDataset)
	public:
		PlotDataset(PlotBase* _ownerPlot, const Plot1DCurveCfg& _config, PlotDatasetData&& _data);
		virtual ~PlotDataset();

		void attach(void);

		void detach(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// General Setter/Getter
		
		const Plot1DCurveCfg& getConfig(void) const { return m_config; };

		void setOwnerPlot(PlotBase* _ownerPlot);

		void setEntityName(const std::string& _name) { m_config.setEntityName(_name); };
		const std::string& getEntityName(void) const { return m_config.getEntityName(); };

		void setEntityID(UID _id) { m_config.setEntityID(_id); };
		UID getEntityID(void) const { return m_config.getEntityID(); };

		void setEntityVersion(UID _version) { m_config.setEntityVersion(_version); };
		UID getEntityVersion(void) const { return m_config.getEntityVersion(); };

		void setEntityInformation(const BasicEntityInformation& _info) { m_config = _info; };
		const BasicEntityInformation& getBasicEntityInformation(void) const { return m_config; };

		void setStorageEntityInfo(const BasicEntityInformation& _entityInfo) { m_storageEntityInfo = _entityInfo; };
		const BasicEntityInformation& getStorageEntityInfo(void) const { return m_storageEntityInfo; };

		void setCurveIsVisibile(bool _isVisible = true, bool _repaint = true);
		bool getCurveIsVisible(void) const { return m_config.getVisible(); };

		void setCurveWidth(double _penSize, bool _repaint = true);

		void setCurveColor(const Color& _color, bool _repaint = true);

		void setCurvePointsVisible(bool _isVisible = true, bool _repaint = true);

		void setCurvePointInnerColor(const Color& _color, bool _repaint = true);

		void setCurvePointOuterColor(const Color& _color, bool _repaint = true);

		void setCurvePointSize(int _size, bool _repaint = true);

		void setCurvePointOuterColorWidth(double _size, bool _repaint = true);

		void setCurveTitle(const std::string& _title);

		void setDimmed(bool _isDimmed, bool _repaint = true);

		void calculateData(Plot1DCfg::AxisQuantity _axisQuantity);

		void setNavigationId(UID _id) { m_config.setNavigationId(_id); };
		UID getNavigationId(void) const { return m_config.getNavigationId(); };

		QwtPlotCurve* getCartesianCurve(void) const { return m_cartesianCurve; };
		QwtPolarCurve* getPolarCurve(void) const { return m_polarCurve; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Setter / Getter

		void replaceData(double* _dataX, double* _dataY, long _dataSize);

		bool getDataAt(int _index, double& _x, double& _y);

		void setYim(double* _dataYim) { m_data.setDataYim(_dataYim); };
		double* getYim(void) const { return m_data.getDataYim(); };

		bool getCopyOfData(double*& _x, double*& _y, long& _size);

		bool getCopyOfYim(double*& _yim, long& _size);

		void updateCurveVisualization(void);

	private:
		friend class PlotBase;

		// Plot data

		PlotBase* m_ownerPlot;

		bool m_isAttatched;

		PlotDatasetData m_data;
		PolarPlotData* m_polarData;

		// Plot elements

		QwtPlotCurve* m_cartesianCurve;
		QwtPolarCurve* m_polarCurve;

		QwtSymbol* m_cartesianCurvePointSymbol;
		QwtSymbol* m_polarCurvePointSymbol;

		// Config

		Plot1DCurveCfg m_config;
		BasicEntityInformation m_storageEntityInfo;
	};

}