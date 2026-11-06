// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qgridlayout.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT GridLayout : public QGridLayout
	{
		OT_DECL_NOCOPY(GridLayout)
		OT_DECL_NOMOVE(GridLayout)
	public:
		explicit GridLayout(QWidget* _parent = nullptr);
		virtual ~GridLayout() = default;
	};
}