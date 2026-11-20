// @otlicense

// GDS header
#include "AuthAPI.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

bool AuthAPI::isGatewayTicketValid(const std::string& _user, const std::string& _ticket) const
{
	if (!isValid())
	{
		OT_LOG_E("Authorization service URL is not set");
		return false;
	}

	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_AuthorizeGateway, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Username, ot::JsonString(_user, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Ticket, ot::JsonString(_ticket, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	ot::ReturnMessage response;
	if (!ot::msg::send("", m_url, ot::EXECUTE_ONE_WAY_TLS, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
	{
		OT_LOG_E("Failed to send authorization request to authorization service");
		return false;
	}

	return response.isTrue();
}
