// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qpoint.h>

class QWidget;

namespace ot {

	class MenuAction;
	class MenuManagerHandler;
	class MenuRequestWidgetEvent;

	class OT_WIDGETS_API_EXPORT MenuCallbackBase {
		OT_DECL_DEFCOPY(MenuCallbackBase)
		OT_DECL_DEFMOVE(MenuCallbackBase)
	public:
		MenuCallbackBase() = default;
		virtual ~MenuCallbackBase() = default;

		bool handleMenuActionTriggered(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

		virtual QWidget* getMenuWidget() const = 0;

		virtual QPoint mapPositionToGlobal(const MenuRequestWidgetEvent* _event) const = 0;

	protected:
		virtual bool menuActionClear(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

		virtual bool menuActionCopy(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);
		virtual bool menuActionCut(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);
		virtual bool menuActionPaste(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

		virtual bool menuActionRename(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

		virtual bool menuActionSelect(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);
		virtual bool menuActionSelectAll(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

	private:
		bool handleNotifyOwnerAction(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);
		bool handleTriggerButtonAction(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler);

	};
} // namespace ot