// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/ContextRequestData.h"
#include "OTWidgets/Event/WidgetEvent.h"

// Qt header
#include <QtCore/qpoint.h>

namespace ot {
	
	class ContextMenuCallbackBase;
	class ContextMenuManagerHandler;

	class ContextRequestWidgetEvent : public WidgetEvent
	{
		OT_DECL_NOCOPY(ContextRequestWidgetEvent)
		OT_DECL_NOMOVE(ContextRequestWidgetEvent)
		OT_DECL_NODEFAULT(ContextRequestWidgetEvent)
	public:
		ContextRequestWidgetEvent(ContextMenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~ContextRequestWidgetEvent();
		
		//! @brief Create a new instance of the context request data for this event. 
		//! Caller takes ownership of the created instance.
		virtual ContextRequestData* createRequestData(const ContextMenuManagerHandler* _handler) const = 0;

		ContextMenuCallbackBase* getContextMenuCallback() const { return m_callbackBase; };

		void setPosition(const QPoint& _pos) { m_pos = _pos; };
		inline const QPoint& getPosition() const { return m_pos; };

	private:
		ContextMenuCallbackBase* m_callbackBase;
		QPoint m_pos;
	};

} // namespace ot