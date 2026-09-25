// @otlicense

// OpenTwin header
#include "OTCore/ThisService.h"
#include "OTGuiAPI/DialogHandler.h"
#include "OTCommunication/ActionTypes.h"

ot::JsonDocument ot::DialogHandler::createDialogRequest(const PropertyDialogCfg& _cfg, const std::string& _callbackAction)
{
	return createDialogRequest(&_cfg, OT_ACTION_CMD_UI_PropertyDialog, _callbackAction, std::nullopt);
}

ot::JsonDocument ot::DialogHandler::createDialogRequest(const DialogCfg* _cfg, const std::string& _requestAction, const std::string& _callbackAction, const std::optional<std::string>& _additionalInfo)
{
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(_requestAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(_cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, JsonString(_callbackAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	if (_additionalInfo.has_value()) {
		doc.AddMember(OT_ACTION_PARAM_Info, JsonString(_additionalInfo.value(), doc.GetAllocator()), doc.GetAllocator());
	}

	return doc;
}
