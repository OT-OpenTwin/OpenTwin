//! @file Plot.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Plot1DCfg.h"
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

		virtual QWidget* getQWidget(void) override { return m_centralWidget; }

		CartesianPlot* getCartesianPlot(void) { return m_cartesianPlot; };

		PolarPlot* getPolarPlot(void) { return m_polarPlot; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		void setPlotType(Plot1DCfg::PlotType _type);

		PlotDataset* addDataset(
			const QString& _title,
			double* _dataX,
			double* _dataY,
			long					_dataSize
		);

		void setFromConfig(const Plot1DCfg& _config);

		void resetView(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		void setErrorState(bool _isError, const QString& _message = QString());

		void setIncompatibleData(void);

		void refresh(void);

		void clear(bool _clearCache);

		void removeFromCache(unsigned long long _entityID);

		void datasetSelectionChanged(PlotDataset* _selectedDataset);

		PlotDataset* findDataset(QwtPlotCurve* _curve);

		PlotDataset* findDataset(QwtPolarCurve* _curve);

		void setAxisQuantity(Plot1DCfg::AxisQuantity _quantity);

	Q_SIGNALS:
		void resetItemSelectionRequest(void);
		void setItemSelectedRequest(UID _treeItemUid, bool _hasControlModifier);

	protected:
		virtual void importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _curvesToImport) {};

		std::map<UID, std::pair<UID, PlotDataset*>>& getCache(void) { return m_cache; };
		const std::map<UID, std::pair<UID, PlotDataset*>>& getCache(void) const { return m_cache; };

	private:
		QWidget* m_centralWidget;
		QLabel* m_errorLabel;
		QVBoxLayout* m_centralLayout;

		CartesianPlot* m_cartesianPlot;
		PolarPlot* m_polarPlot;

		Plot1DCfg::PlotType			m_currentPlotType;

		bool							m_isError;

		unsigned long					m_currentDatasetId;

		std::map<UID, std::pair<UID, PlotDataset*>> m_cache;

	};

}