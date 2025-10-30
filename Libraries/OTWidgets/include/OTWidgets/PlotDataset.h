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
#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTWidgets/PlotDatasetData.h"
#include "OTWidgets/CoordinateFormatConverter.h"

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

		void setCurvePointInnerColor(const Color& _color, bool _repaint = true);

		void setCurvePointOuterColor(const Color& _color, bool _repaint = true);

		void setCurvePointSize(int _size, bool _repaint = true);

		void setCurvePointOuterColorWidth(double _size, bool _repaint = true);

		void setDimmed(bool _isDimmed, bool _repaint = true);

		void setPointInterval(int _interval, bool _repaint = true);

		void setConfig(ot::Plot1DCurveCfg& _config) {
			m_config = _config;
		}

		void setSelected(bool _isSelected);
		bool isSelected() const { return m_isSelected; };

		void setNavigationId(UID _id) { m_config.setNavigationId(_id); };
		UID getNavigationId(void) const { return m_config.getNavigationId(); };

		CartesianPlotCurve* getCartesianCurve();
		PolarPlotCurve* getPolarCurve();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data Setter / Getter
		void updateCurveVisualization(void);
		PlotDatasetData& getPlotData() { return m_data; }
		const PointsContainer getDisplayedPoints();
		void setPlotData(PlotDatasetData&& _dataset) { m_data = std::move(_dataset); }

		void setCurveNameBase(const std::string& _curveNameBase) { m_curveNameBase = _curveNameBase; }
		const std::string& getCurveNameBase() { return m_curveNameBase; }

	private:
		friend class PlotBase;

		std::string m_curveNameBase = "";

		void buildCartesianCurve();
		void buildPolarCurve();

		PlotBase* m_ownerPlot = nullptr;

		bool m_isAttatched = false;
		bool m_isSelected = false;
		
		PlotDatasetData m_data;
		CoordinateFormatConverter m_coordinateFormatConverter;

		// Plot elements
		CartesianPlotCurve* m_cartesianCurve = nullptr;

		PolarPlotCurve* m_polarCurve = nullptr;
		PolarPlotData* m_polarData = nullptr; //Adapter pattern, since the QwtPolarCurve requires data of a internal type. 

		QwtSymbol* m_cartesianCurvePointSymbol = nullptr; //Ownership is taken by the QwtPlotCurve that get the symbol set in setSymbol
		QwtSymbol* m_polarCurvePointSymbol = nullptr;

		// Config

		Plot1DCurveCfg m_config;
		BasicEntityInformation m_storageEntityInfo;
	};

}