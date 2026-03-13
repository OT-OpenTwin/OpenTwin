// @otlicense

// OpenTwin header
#include "OTWidgets/Menu/MenuCallbackBase.h"
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

ot::MenuRequestWidgetEvent::MenuRequestWidgetEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos)
	: WidgetEvent(_callbackBase->getMenuWidget()), m_callbackBase(_callbackBase), m_pos(_pos)
{
	OTAssertNullptr(_callbackBase);
}

ot::MenuRequestWidgetEvent::~MenuRequestWidgetEvent()
{

}
