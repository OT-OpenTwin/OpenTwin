// @otlicense
// File: PlotBase.h
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
#include "OTGui/Plot/Plot1DDataBaseCfg.h"
#include "OTWidgets/Plot/AbstractPlot.h"

// Qwt header
#include <qwt_point_polar.h>

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtGui/qpen.h>

// std header
#include <map>

class QVBoxLayout;

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;
class QwtPointPolar;

namespace ot {

	class Label;
	class PolarPlot;
	class PlotLegend;
	class PlotDataset;
	class CartesianPlot;
	class PolarPlotData;

	class OT_WIDGETS_API_EXPORT PlotBase : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotBase)
		OT_DECL_NOMOVE(PlotBase)
		OT_DECL_NODEFAULT(PlotBase)
	public:
		explicit PlotBase(QWidget* _parent);
		virtual ~PlotBase();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual QWidget* getQWidget() override { return m_centralWidget; };
		virtual const QWidget* getQWidget() const override { return m_centralWidget; };

		AbstractPlot* getPlot();
		
		PlotLegend* getLegend() const { return m_legend; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		void setPlotType(Plot1DCfg::PlotType _type);
		Plot1DCfg::PlotType getCurrentPlotType() { return m_currentPlotType; }

		void setConfig(Plot1DCfg&& _config) { m_config = std::move(_config); };
		const Plot1DCfg& getConfig() const { return m_config; };

		void resetView();

		void renameDataset(const std::string& _oldEntityPath, const std::string& _newEntityPath);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		void setErrorState(bool _isError, const QString& _message = QString());

		void setIncompatibleData();

		void refresh();

		void clear(bool _clearCache);

		//! @brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual PlotDataset* findDataset(QwtPlotCurve* _curve) = 0;

		//! @brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual PlotDataset* findDataset(QwtPolarCurve* _curve) = 0;

		//! @brief Returns the cached dataset for the given entity id.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual std::list<PlotDataset*> findDatasets(UID _entityID) = 0;
		
		virtual void addDatasetToCache(PlotDataset* _dataset) = 0;

		//! @brief Returns all datasets.
		virtual std::list<PlotDataset*> getAllDatasets() const = 0;

		//! @brief Sets the selected state of the curves in the plot.
		//! Curves not in the list will be deselected.
		//! @param _selectedCurves List of selected curves (by UID).
		void setSelectedCurves(const UIDList& _selectedCurves);

		void requestResetItemSelection();
		void requestCurveDoubleClicked(UID _entityID, bool _hasControlModifier);

		void setInfoText(const QString& _text);
		void clearPositionInfoText();
		void setInfoTextFromPosition(const QPoint& _pos);
		void setInfoTextFromPosition(const QPointF& _pos);
		void setInfoTextFromPosition(const QwtPointPolar& _pos);

		QString toPositionInfoText(const QPoint& _pos, bool _multiline) const;
		QString toPositionInfoText(const QPointF& _pos, bool _multiline) const;
		QString toPositionInfoText(const QwtPointPolar& _pos, bool _multiline) const;

	Q_SIGNALS:
		void resetItemSelectionRequest();
		void curveDoubleClicked(UID _entityID, bool _hasControlModifier);

	protected:
		virtual void clearCache() = 0;
		virtual void detachAllCached() = 0;
	
		void replaceConfig(Plot1DCfg&& _config) { m_config = std::move(_config); };
		void applyConfig();
		void updateAxisTitles(bool _replot = false);
		
	private:
		Plot1DCfg m_config;

		QWidget* m_centralWidget;
		Label* m_errorLabel;
		QVBoxLayout* m_plotLayout;

		QString m_infoText;
		Label* m_infoLabel;

		QWidget* m_legendContainer;
		PlotLegend* m_legend;

		CartesianPlot* m_cartesianPlot;
		PolarPlot* m_polarPlot;

		Plot1DCfg::PlotType m_currentPlotType;

		bool m_isError;
		
	};

}