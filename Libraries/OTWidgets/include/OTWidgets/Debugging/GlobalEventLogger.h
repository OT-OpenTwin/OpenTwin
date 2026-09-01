// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtGui/qevent.h>

namespace ot
{
	
	class OT_WIDGETS_API_EXPORT GlobalEventLogger : public QObject
	{
		OT_DECL_NOCOPY(GlobalEventLogger)
		OT_DECL_NOMOVE(GlobalEventLogger)
		OT_DECL_NODEFAULT(GlobalEventLogger)
	public:
		static void install(const std::initializer_list<QEvent::Type>& _eventTypes);

		virtual bool eventFilter(QObject* _obj, QEvent* _event) override;
	private:
		explicit GlobalEventLogger(const std::initializer_list<QEvent::Type>& _eventTypes);

		bool m_enabledEventTypes[static_cast<int>(QEvent::User)];
	};

}
