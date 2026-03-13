// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/PlainTextEdit.h"
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEditorMenuRequestEvent : public MenuRequestWidgetEvent
	{
		OT_DECL_NOCOPY(TextEditorMenuRequestEvent)
		OT_DECL_NOMOVE(TextEditorMenuRequestEvent)
		OT_DECL_NODEFAULT(TextEditorMenuRequestEvent)
	public:
		TextEditorMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~TextEditorMenuRequestEvent() = default;

		//! @brief Create a new instance of the menu request data for this event. 
		//! Caller takes ownership of the created instance.
		virtual MenuRequestData* createRequestData(const MenuManagerHandler* _handler) const override;

		OT_DECL_NODISCARD PlainTextEdit* getPlainTextEdit() const { return this->getWidgetAs<PlainTextEdit>(); };

	};
} // namespace ot