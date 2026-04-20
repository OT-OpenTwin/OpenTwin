// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qscrollarea.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ScrollArea : public QScrollArea, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(ScrollArea)
		OT_DECL_NOMOVE(ScrollArea)
		OT_DECL_NODEFAULT(ScrollArea)
	public:
		explicit ScrollArea(QWidget* _parent);
		virtual ~ScrollArea() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
	};

}