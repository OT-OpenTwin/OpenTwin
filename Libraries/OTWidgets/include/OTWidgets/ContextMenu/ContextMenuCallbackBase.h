// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

class QWidget;

namespace ot {

	class ContextMenuAction;
	class ContextRequestWidgetEvent;
	class ContextMenuManagerHandler;

	class OT_WIDGETS_API_EXPORT ContextMenuCallbackBase {
		OT_DECL_DEFCOPY(ContextMenuCallbackBase)
		OT_DECL_DEFMOVE(ContextMenuCallbackBase)
	public:
		ContextMenuCallbackBase() = default;
		virtual ~ContextMenuCallbackBase() = default;

		bool handleContextActionTriggered(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

		virtual QWidget* getContextWidget() const = 0;

	protected:
		virtual bool contextActionClear(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

		virtual bool contextActionCopy(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);
		virtual bool contextActionCut(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);
		virtual bool contextActionPaste(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

		virtual bool contextActionRename(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

		virtual bool contextActionSelect(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);
		virtual bool contextActionSelectAll(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

	private:
		bool handleNotifyOwnerAction(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);
		bool handleTriggerButtonAction(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler);

	};
} // namespace ot