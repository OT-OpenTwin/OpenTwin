// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Menu/MenuButtonCfg.h"
#include "OTWidgets/ContextMenu/ContextMenuAction.h"
#include "OTWidgets/ContextMenu/ContextMenuManager.h"
#include "OTWidgets/ContextMenu/ContextMenuCallbackBase.h"
#include "OTWidgets/ContextMenu/ContextMenuManagerHandler.h"

bool ot::ContextMenuCallbackBase::handleContextActionTriggered(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _requestEvent, const ContextMenuManagerHandler* _handler)
{
	OTAssertNullptr(_action);
	OTAssertNullptr(_handler);

	const MenuButtonCfg& config = _action->getConfiguration();
	switch (config.getButtonAction()) {
	case MenuButtonCfg::ButtonAction::Clear: return this->contextActionClear(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Copy: return this->contextActionCopy(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Cut: return this->contextActionCut(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Paste: return this->contextActionPaste(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Rename: return this->contextActionRename(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::Select: return this->contextActionSelect(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::SelectAll: return this->contextActionSelectAll(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::TriggerButton: return this->handleTriggerButtonAction(_action, _requestEvent, _handler); break;
	case MenuButtonCfg::ButtonAction::NotifyOwner: return this->handleNotifyOwnerAction(_action, _requestEvent, _handler); break;
	default:
		OT_LOG_E("Unknown context menu action \"" + MenuButtonCfg::toString(config.getButtonAction()) + "\"");
		return false;
	}
}

bool ot::ContextMenuCallbackBase::contextActionClear(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Clear) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionCopy(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Copy) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionCut(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Cut) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionPaste(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Paste) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionRename(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Rename) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionSelect(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::Select) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::contextActionSelectAll(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	OT_LOG_E("Context action \"" + MenuButtonCfg::toString(MenuButtonCfg::SelectAll) + "\" is not implemented for this widget");
	return false;
}

bool ot::ContextMenuCallbackBase::handleNotifyOwnerAction(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	return _handler->notifyContextOwner(_action);
}

bool ot::ContextMenuCallbackBase::handleTriggerButtonAction(const ContextMenuAction* _action, const ContextRequestWidgetEvent* _event, const ContextMenuManagerHandler* _handler)
{
	return _handler->triggerToolbarButton(_action->getConfiguration().getTriggerButton());
}
;