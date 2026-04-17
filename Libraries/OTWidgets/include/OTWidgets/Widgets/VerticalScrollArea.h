// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qscrollarea.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT VerticalScrollArea : public QScrollArea, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(VerticalScrollArea)
		OT_DECL_NOMOVE(VerticalScrollArea)
		OT_DECL_NODEFAULT(VerticalScrollArea)
	public:
		explicit VerticalScrollArea(QWidget* _parent);
		virtual ~VerticalScrollArea() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
	};

}