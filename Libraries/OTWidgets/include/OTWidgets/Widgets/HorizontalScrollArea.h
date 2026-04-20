// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/ScrollArea.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT HorizontalScrollArea : public ScrollArea
	{
		Q_OBJECT
		OT_DECL_NOCOPY(HorizontalScrollArea)
		OT_DECL_NOMOVE(HorizontalScrollArea)
		OT_DECL_NODEFAULT(HorizontalScrollArea)
	public:
		explicit HorizontalScrollArea(QWidget* _parent);
		virtual ~HorizontalScrollArea() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;
	};

}