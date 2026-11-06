// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qboxlayout.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT VBoxLayout : public QVBoxLayout
	{
		OT_DECL_NOCOPY(VBoxLayout)
		OT_DECL_NOMOVE(VBoxLayout)
	public:
		explicit VBoxLayout(QWidget* _parent = nullptr);
		virtual ~VBoxLayout() = default;
	};
}