// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class Label;
	class Painter2D;
	class PlotLegend;
	class Painter2DPreview;

	class OT_WIDGETS_API_EXPORT PlotLegendItem : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegendItem)
		OT_DECL_NOMOVE(PlotLegendItem)
	public:
		explicit PlotLegendItem();
		virtual ~PlotLegendItem();

		QWidget* getItemWidget() const { return m_itemWidget; };

		void setLegend(PlotLegend* _legend) { m_legend = _legend; };
		PlotLegend* getLegend() const { return m_legend; };
		void forgetLegend() { m_legend = nullptr; };

		void setLabel(const QString& _text);
		void setPainter(const ot::Painter2D* _painter);

		void attach();
		void detach();

	private:
		PlotLegend* m_legend;

		QWidget* m_itemWidget;
		Label* m_label;
		Painter2DPreview* m_colorPreview;
	};

}
