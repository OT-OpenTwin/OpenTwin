// @otlicense

// OpenTwin header
#include "OTGui/Event/TextEditorMenuRequestData.h"
#include "OTWidgets/Event/TextEditorMenuRequestEvent.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"
#include "OTWidgets/WidgetView/WidgetView.h"

ot::TextEditorMenuRequestEvent::TextEditorMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos)
	: MenuRequestWidgetEvent(_callbackBase, _pos)
{
	OTAssertNullptr(getPlainTextEdit());
}

ot::MenuRequestData* ot::TextEditorMenuRequestEvent::createRequestData(const MenuManagerHandler* _handler) const
{
	BasicEntityInformation entityInfo;
	PlainTextEdit* textEdit = this->getPlainTextEdit();
	OTAssertNullptr(textEdit);

	const WidgetView* parentView = textEdit->getParentWidgetView();
	if (parentView)
	{
		entityInfo = parentView->getViewData();
	}

	return new TextEditorMenuRequestData(entityInfo);
}
