// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/ScrollArea.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT VerticalScrollArea : public ScrollArea
	{
		Q_OBJECT
		OT_DECL_NOCOPY(VerticalScrollArea)
		OT_DECL_NOMOVE(VerticalScrollArea)
		OT_DECL_NODEFAULT(VerticalScrollArea)
	public:
		explicit VerticalScrollArea(QWidget* _parent);
		virtual ~VerticalScrollArea() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;
	};

}