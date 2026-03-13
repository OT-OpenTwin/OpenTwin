// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Menu/MenuButtonCfg.h"
#include "OTWidgets/Menu/MenuAction.h"
#include "OTWidgets/Menu/MenuManager.h"
#include "OTWidgets/Menu/MenuCallbackBase.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"

bool ot::MenuCallbackBase::handleMenuActionTriggered(const MenuAction* _action, const MenuRequestWidgetEvent* _requestEvent, const MenuManagerHandler* _handler)
{
	OTAssertNullptr(_action);
	OTAssertNullptr(_handler);

	const MenuButtonCfg& config = _action->getConfiguration();
	switch (config.getButtonAction()) {
	case MenuButtonCfg::ButtonAction::Clear: return this->menuActionClear(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Copy: return this->menuActionCopy(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Cut: return this->menuActionCut(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Paste: return this->menuActionPaste(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Rename: return this->menuActionRename(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Select: return this->menuActionSelect(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::SelectAll: return this->menuActionSelectAll(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::TriggerButton: return this->handleTriggerButtonAction(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::NotifyOwner: return this->handleNotifyOwnerAction(_action, _requestEvent, _handler); break;
	default:
		OT_LOG_E("Unknown menu action \"" + MenuButtonCfg::toString(config.getButtonAction()) + "\"");
		return false;
	}
}

bool ot::MenuCallbackBase::menuActionClear(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Clear) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionCopy(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Copy) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionCut(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Cut) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionPaste(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Paste) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionRename(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Rename) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionSelect(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::Select) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::menuActionSelectAll(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	OT_LOG_E("Menu action \"" + MenuButtonCfg::toString(MenuButtonCfg::SelectAll) + "\" is not implemented for this widget");
	return false;
}

bool ot::MenuCallbackBase::handleNotifyOwnerAction(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	return _handler->notifyMenuOwner(_action);
}

bool ot::MenuCallbackBase::handleTriggerButtonAction(const MenuAction* _action, const MenuRequestWidgetEvent* _event, const MenuManagerHandler* _handler)
{
	return _handler->triggerToolbarButton(_action->getConfiguration().getTriggerButton());
}
;