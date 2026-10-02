// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot
{

	class OT_WIDGETS_API_EXPORT WidgetDebugHelper
	{
		OT_DECL_STATICONLY(WidgetDebugHelper)
	public:
		static std::string objectHierarchyString(QObject* _object, const std::string& _linePrefix = std::string());

	};

}