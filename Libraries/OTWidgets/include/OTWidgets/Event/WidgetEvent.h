// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

class QWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT WidgetEvent
	{
		OT_DECL_NOCOPY(WidgetEvent)
		OT_DECL_NOMOVE(WidgetEvent)
	public:
		WidgetEvent(QWidget* _widget = (QWidget*)nullptr);
		virtual ~WidgetEvent() = default;
		
		void setWidget(QWidget* _widget) { m_widget = _widget; };
		QWidget* getWidget() const { return m_widget; }

	protected:
		template<typename T>
		T* getWidgetAs() const
		{
			OTAssertNullptr(dynamic_cast<T*>(m_widget));
			return static_cast<T*>(m_widget);
		};

	private:
		QWidget* m_widget = nullptr;

	};

} // namespace ot