// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qslider.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT Slider : public QSlider, public WidgetBase
	{
		OT_DECL_NOCOPY(Slider)
		OT_DECL_NOMOVE(Slider)
	public:
		explicit Slider(QWidget* _parent = nullptr);
		explicit Slider(Qt::Orientation _orientation, QWidget* _parent = nullptr);
		virtual ~Slider() = default;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
		
	};
}