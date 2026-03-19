// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class Painter2D;
	class PlotLegend;
	class PlotDataset;
	class PlotLegendItemWidget;

	class OT_WIDGETS_API_EXPORT PlotLegendItem : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegendItem)
		OT_DECL_NOMOVE(PlotLegendItem)
		OT_DECL_NODEFAULT(PlotLegendItem)
	public:
		explicit PlotLegendItem(PlotDataset* _dataset);
		virtual ~PlotLegendItem();

		QWidget* getSelectedWidget() const;
		QWidget* getDimmedWidget() const;

		void updateVisibility();

		void setLegend(PlotLegend* _legend) { m_legend = _legend; };
		PlotLegend* getLegend() const { return m_legend; };
		void forgetLegend() { m_legend = nullptr; };

		void setLabel(const QString& _text);
		void setSelectedPainter(const ot::Painter2D* _painter);
		void setDimmedPainter(const ot::Painter2D* _painter);

		void attach();
		void detach();
		
		PlotDataset* getDataset() const { return m_dataset; };

	private:
		PlotLegend* m_legend;
		PlotDataset* m_dataset;

		PlotLegendItemWidget* m_selectedWidget;
		PlotLegendItemWidget* m_dimmedWidget;

	};

}
