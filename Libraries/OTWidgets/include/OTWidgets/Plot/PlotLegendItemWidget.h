// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {
	
	class Label;
	class Painter2D;
	class Painter2DPreview;

	class OT_WIDGETS_API_EXPORT PlotLegendItemWidget : public QObject, public WidgetBase
	{
		OT_DECL_NOCOPY(PlotLegendItemWidget)
		OT_DECL_NOMOVE(PlotLegendItemWidget)
	public:
		PlotLegendItemWidget();
		virtual ~PlotLegendItemWidget();

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

		void setLabel(const QString& _text);
		void setPainter(const ot::Painter2D* _painter);

	private:
		QWidget* m_rootWidget;

		Painter2DPreview* m_colorPreview;
		Label* m_label;

	};
}