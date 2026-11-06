// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qboxlayout.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT HBoxLayout : public QHBoxLayout
	{
		OT_DECL_NOCOPY(HBoxLayout)
		OT_DECL_NOMOVE(HBoxLayout)
	public:
		explicit HBoxLayout(QWidget* _parent = nullptr);
		virtual ~HBoxLayout() = default;
	};
}