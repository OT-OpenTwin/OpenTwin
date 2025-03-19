//! @file PlotBase.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DDataBaseCfg.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/AbstractPlot.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtGui/qpen.h>

// std header
#include <map>

class QWidget;
class QLabel;
class QVBoxLayout;

class QwtSymbol;
class QwtPlotCurve;
class QwtPolarCurve;


namespace ot {

	class PolarPlot;
	class PlotDataset;
	class CartesianPlot;
	class PolarPlotData;

	class OT_WIDGETS_API_EXPORT PlotBase : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotBase)
		OT_DECL_NOMOVE(PlotBase)
	public:
		PlotBase();
		virtual ~PlotBase();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		virtual QWidget* getQWidget(void) override { return m_centralWidget; };
		virtual const QWidget* getQWidget(void) const override { return m_centralWidget; };

		CartesianPlot* getCartesianPlot(void) { return m_cartesianPlot; };

		PolarPlot* getPolarPlot(void) { return m_polarPlot; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		void setPlotType(Plot1DCfg::PlotType _type);

		PlotDataset* addDataset(const Plot1DCurveCfg& _config, double* _dataX, double* _dataY, int _dataSize);

		void setConfig(const Plot1DCfg& _config) { m_config = _config; };
		const Plot1DCfg& getConfig(void) const { return m_config; };

		void resetView(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		void setErrorState(bool _isError, const QString& _message = QString());

		void setIncompatibleData(void);

		void refresh(void);

		void clear(bool _clearCache);

		void datasetSelectionChanged(PlotDataset* _selectedDataset);

		//! \brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual PlotDataset* findDataset(QwtPlotCurve* _curve) = 0;

		//! \brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual PlotDataset* findDataset(QwtPolarCurve* _curve) = 0;

		//! \brief Returns the cached dataset for the given entity id.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		virtual std::list<PlotDataset*> findDatasets(UID _entityID) = 0;
		
		virtual void addDatasetToCache(PlotDataset* _dataset) = 0;

		//! @brief Returns all datasets.
		virtual std::list<PlotDataset*> getAllDatasets(void) const = 0;

		void setAxisQuantity(Plot1DCfg::AxisQuantity _quantity);

	Q_SIGNALS:
		void resetItemSelectionRequest(void);
		void setItemSelectedRequest(UID _treeItemUid, bool _hasControlModifier);

	protected:
		virtual void clearCache(void) = 0;
		virtual void detachAllCached(void) = 0;
		virtual void calculateDataInCache(Plot1DCfg::AxisQuantity _axisQuantity) = 0;

		void replaceConfig(Plot1DCfg&& _config) { m_config = std::move(_config); };
		void applyConfig(void);
		
	private:
		Plot1DCfg m_config;

		QWidget* m_centralWidget;
		QLabel* m_errorLabel;
		QVBoxLayout* m_centralLayout;

		CartesianPlot* m_cartesianPlot;
		PolarPlot* m_polarPlot;

		Plot1DCfg::PlotType m_currentPlotType;

		bool m_isError;
		
	};

}