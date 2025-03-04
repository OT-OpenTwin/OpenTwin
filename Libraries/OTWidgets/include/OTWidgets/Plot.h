//! @file Plot.h
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

	class OT_WIDGETS_API_EXPORT Plot : public QObject, public QWidgetInterface {
		Q_OBJECT
	public:
		Plot();
		virtual ~Plot();

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

		void setFromDataBaseConfig(const Plot1DDataBaseCfg& _config);

		void resetView(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		void setErrorState(bool _isError, const QString& _message = QString());

		void setIncompatibleData(void);

		void refresh(void);

		void clear(bool _clearCache);

		void removeFromCache(unsigned long long _entityID);

		void datasetSelectionChanged(PlotDataset* _selectedDataset);

		//! \brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		PlotDataset* findDataset(QwtPlotCurve* _curve);

		//! \brief Returns the dataset for the given plot curve.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		PlotDataset* findDataset(QwtPolarCurve* _curve);

		//! \brief Returns the cached dataset for the given entity id.
		//! Returns NULL if the dataset does not exist.
		//! The Plot keeps ownership of the dataset.
		PlotDataset* findDataset(UID _entityID);

		void setAxisQuantity(Plot1DCfg::AxisQuantity _quantity);

		bool hasCachedEntity(UID _entityID) const;

		//! \brief Updates the entity version of the specified dataset if needed.
		//! Returns false if the dataset does not exist or the version did not change.
		bool changeCachedDatasetEntityVersion(UID _entityID, UID _newEntityVersion);

		std::list<PlotDataset*> getDatasets(void) const;

	Q_SIGNALS:
		void resetItemSelectionRequest(void);
		void setItemSelectedRequest(UID _treeItemUid, bool _hasControlModifier);

	protected:
		virtual void importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _curvesToImport) {};

		std::map<UID, std::pair<UID, PlotDataset*>>& getCache(void) { return m_cache; };
		const std::map<UID, std::pair<UID, PlotDataset*>>& getCache(void) const { return m_cache; };

	private:
		void applyConfig(void);

		Plot1DCfg m_config;

		QWidget* m_centralWidget;
		QLabel* m_errorLabel;
		QVBoxLayout* m_centralLayout;

		CartesianPlot* m_cartesianPlot;
		PolarPlot* m_polarPlot;

		Plot1DCfg::PlotType m_currentPlotType;

		bool m_isError;
		
		std::map<UID, std::pair<UID, PlotDataset*>> m_cache;

	};

}