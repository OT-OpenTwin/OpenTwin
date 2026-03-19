// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
class QVBoxLayout;

namespace ot {

	class PlotBase;
	class PlotLegendItem;

	class OT_WIDGETS_API_EXPORT PlotLegend : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegend)
		OT_DECL_NOMOVE(PlotLegend)
		OT_DECL_NODEFAULT(PlotLegend)
	public:
		explicit PlotLegend(PlotBase* _plot, QWidget* _parentWidget);
		virtual ~PlotLegend();

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

		void addItem(PlotLegendItem* _item);
		void removeItem(PlotLegendItem* _item);

	private:
		friend class PlotLegendItem;

		PlotBase* m_plot;
		QWidget* m_rootWidget;
		QVBoxLayout* m_selectedLayout;
		QVBoxLayout* m_dimmedLayout;

		std::list<PlotLegendItem*> m_items;
	};

}