// @otlicense
// File: PlotDataset.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTWidgets/Plot/PlotDatasetData.h"

// Qwt header
#include <qwt_symbol.h>

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>

// std header
#include <string>

class QwtSymbol;

namespace ot {

	class PlotBase;
	class PolarPlotData;
	class PolarPlotCurve;
	class CartesianPlotCurve;

	class OT_WIDGETS_API_EXPORT PlotDataset {
		OT_DECL_NOCOPY(PlotDataset)
		OT_DECL_NODEFAULT(PlotDataset)
	public:
		static QwtSymbol::Style toQwtSymbolStyle(Plot1DCurveCfg::Symbol _symbol);
		static Plot1DCurveCfg::Symbol toPlot1DCurveSymbol(QwtSymbol::Style _symbol);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		PlotDataset(PlotBase* _ownerPlot, const Plot1DCurveCfg& _config, PlotDatasetData&& _data);
		virtual ~PlotDataset();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Attach / Detach

		void attach();

		void detach();

		void rebuildCurve(bool _reattach = false);

		// ###########################################################################################################################################################################################################################################################################################################################

		// General Setter/Getter
		
		const Plot1DCurveCfg& getConfig() const { return m_config; };

		void setOwnerPlot(PlotBase* _ownerPlot);

		void setEntityName(const std::string& _name) { m_config.setEntityName(_name); };
		const std::string& getEntityName() const { return m_config.getEntityName(); };

		void setEntityID(UID _id) { m_config.setEntityID(_id); };
		UID getEntityID() const { return m_config.getEntityID(); };

		void setEntityVersion(UID _version) { m_config.setEntityVersion(_version); };
		UID getEntityVersion() const { return m_config.getEntityVersion(); };

		void setEntityInformation(const BasicEntityInformation& _info) { m_config = _info; };
		const BasicEntityInformation& getBasicEntityInformation() const { return m_config; };

		void setStorageEntityInfo(const BasicEntityInformation& _entityInfo) { m_storageEntityInfo = _entityInfo; };
		const BasicEntityInformation& getStorageEntityInfo() const { return m_storageEntityInfo; };

		void setCurveIsVisibile(bool _isVisible = true, bool _repaint = true);
		bool getCurveIsVisible() const { return m_config.getVisible(); };

		void setCurveWidth(double _penSize, bool _repaint = true);

		void setCurveColor(const Color& _color, bool _repaint = true);

		void setCurvePointInnerColor(const Color& _color, bool _repaint = true);

		void setCurvePointOuterColor(const Color& _color, bool _repaint = true);

		void setCurvePointSize(int _size, bool _repaint = true);

		void setCurvePointOuterColorWidth(double _size, bool _repaint = true);

		void setDimmed(bool _isDimmed, bool _repaint = true);

		void setPointInterval(int _interval, bool _repaint = true);

		void setConfig(ot::Plot1DCurveCfg& _config) {
			m_config = _config;
		}

		void setAxisQuantitiesAndScaling(Plot1DAxisCfg::AxisQuantity _xQuantity, const Plot1DAxisCfg::QuantityScaling& _xQuantityScaling, Plot1DAxisCfg::AxisQuantity _yQuantity, const Plot1DAxisCfg::QuantityScaling& _yQuantityScaling);

		void setSelected(bool _isSelected);
		bool isSelected() const { return m_isSelected; };

		void setNavigationId(UID _id) { m_config.setNavigationId(_id); };
		UID getNavigationId() const { return m_config.getNavigationId(); };

		CartesianPlotCurve* getCartesianCurve();
		PolarPlotCurve* getPolarCurve();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Setter / Getter

		void updateCurveVisualization();
		PlotDatasetData& getPlotData() { return m_data; }
		void setPlotData(PlotDatasetData&& _dataset) { m_data = std::move(_dataset); }

		void setCurveNameBase(const std::string& _curveNameBase) { m_curveNameBase = _curveNameBase; }
		const std::string& getCurveNameBase() { return m_curveNameBase; }

		inline bool canConvert() const { return m_data.canConvert(); };

	private:
		friend class PlotBase;

		std::string m_curveNameBase = "";

		void buildCartesianCurve();
		void buildPolarCurve();

		PlotBase* m_ownerPlot = nullptr;

		bool m_isAttatched = false;
		bool m_isSelected = false;
		
		PlotDatasetData m_data;
		
		// Plot elements
		CartesianPlotCurve* m_cartesianCurve = nullptr;
		QwtSymbol* m_cartesianCurvePointSymbol = nullptr; //Ownership is taken by the QwtPlotCurve that get the symbol set in setSymbol

		PolarPlotCurve* m_polarCurve = nullptr;
		QwtSymbol* m_polarCurvePointSymbol = nullptr;

		// Config
		Plot1DCurveCfg m_config;
		BasicEntityInformation m_storageEntityInfo;
	};

}