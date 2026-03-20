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

	//! @brief The PlotBase class is the base class for all plot widgets.
	//! It provides common functionality for handling plot configuration, data management, and user interactions.
	//! It also defines the interface for finding datasets associated with plot curves and managing the plot legend.
	class OT_WIDGETS_API_EXPORT PlotBase : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotBase)
		OT_DECL_NOMOVE(PlotBase)
		OT_DECL_NODEFAULT(PlotBase)
	public:

		//! @brief Defines the Z-order for plot items.
		struct ItemZOrder
		{
			static constexpr double background() { return 0.0; };
			static constexpr double grid() { return 1.0; };
			static constexpr double dimmedCurves() { return 2.0; };
			static constexpr double visibleCurves() { return 3.0; };
			static constexpr double highlightedCurves() { return 4.0; };
			static constexpr double infoText() { return 5.0; };

		};

		explicit PlotBase(QWidget* _parent);
		virtual ~PlotBase();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Returns the central widget of the plot, which contains the plot area and the legend.
		virtual QWidget* getQWidget() override { return m_centralWidget; };

		//! @brief Returns the central widget of the plot, which contains the plot area and the legend.
		virtual const QWidget* getQWidget() const override { return m_centralWidget; };

		PlotLegend* getLegend() const { return m_legend; };

		void setPlotType(Plot1DCfg::PlotType _type);
		Plot1DCfg::PlotType getCurrentPlotType() { return m_currentPlotType; }

		//! @brief Returns a pointer to the current plot based on the current plot type.
		AbstractPlot* getCurrentPlot();

		//! @brief Set the plot configuration.
		//! Call applyConfig() to apply changes to the plot.
		//! @param _config 
		void setConfig(Plot1DCfg&& _config) { m_config = std::move(_config); };
		const Plot1DCfg& getConfig() const { return m_config; };

		void resetView();

		void renameDataset(const std::string& _oldEntityPath, const std::string& _newEntityPath);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		//! @brief Sets the error state of the plot.
		//! If the plot is in an error state, an error message will be displayed instead of the plot.
		//! @param _isError Indicates whether the plot is in an error state.
		//! @param _message The error message to display if the plot is in an error state.
		void setErrorState(bool _isError, const QString& _message = QString());

		void setIncompatibleData();

		//! @brief Applies the current configuration to the plot, updating its appearance and behavior accordingly.
		void applyConfig();

		//! @brief Replots the plot, refreshing its display to reflect any changes in the data or state.
		void replot();

		//! @brief Clears the plot.
		//! @param _clearCache If true, the cache will be cleared.
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
		
		//! @brief Adds a dataset to the cache.
		virtual void addDatasetToCache(PlotDataset* _dataset) = 0;

		//! @brief Returns all datasets.
		virtual std::list<PlotDataset*> getAllDatasets() const = 0;

		//! @brief Sets the selected state of the curves in the plot.
		//! Curves not in the list will be deselected.
		//! @param _selectedCurves List of selected curves (by UID).
		void setSelectedCurves(const UIDList& _selectedCurves);

		//! @brief Emit the resetItemSelectionRequest() signal.
		void requestResetItemSelection();

		//! @brief Emit the curveDoubleClicked() signal for the given dataset and control modifier state.
		void requestCurveDoubleClicked(PlotDataset* _dataset, bool _hasControlModifier);

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
		void curveDoubleClicked(PlotDataset* _dataset, bool _hasControlModifier);

	protected:
		//! @brief Clears the dataset cache, removing all cached datasets and their associations.
		virtual void clearCache() = 0;

		//! @brief Detaches all cached datasets from their associated plot curves, without removing them from the cache.
		virtual void detachAllCached() = 0;
	
		//! @brief Updates the axis titles of the plot based on the current configuration.
		//! @param _replot If true, the plot will be replotted after updating the axis titles.
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