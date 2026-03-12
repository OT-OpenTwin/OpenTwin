// @otlicense

// OpenTwin header
#include "OTWidgets/ContextMenu/ContextMenuCallbackBase.h"
#include "OTWidgets/Event/ContextRequestWidgetEvent.h"

ot::ContextRequestWidgetEvent::ContextRequestWidgetEvent(ContextMenuCallbackBase* _callbackBase, const QPoint& _pos)
	: WidgetEvent(_callbackBase->getContextWidget()), m_callbackBase(_callbackBase), m_pos(_pos)
{
	OTAssertNullptr(_callbackBase);
}

ot::ContextRequestWidgetEvent::~ContextRequestWidgetEvent()
{

}
