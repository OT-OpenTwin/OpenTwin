// @otlicense

// Frontend header
#include "Login/LoginTicketData.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTCommunication/ActionTypes.h"

#include <QtCore/qurl.h>

void LoginTicketData::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_SERVICE_GSS_Host, ot::JsonString(m_gssHost, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_Ticket, ot::JsonString(m_loginTicket, _allocator), _allocator);
}

void LoginTicketData::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_loginTicket = ot::json::getString(_jsonObject, OT_ACTION_PARAM_Ticket);

	if (_jsonObject.HasMember(OT_ACTION_PARAM_SERVICE_GSS_URL))
	{
		std::string url = ot::json::getString(_jsonObject, OT_ACTION_PARAM_SERVICE_GSS_URL);
		QUrl qurl(QString::fromStdString(url));
		m_gssHost = qurl.host().toStdString();
		m_gssPort = std::to_string(qurl.port());
	}
	else
	{
		m_gssHost = ot::json::getString(_jsonObject, OT_ACTION_PARAM_SERVICE_GSS_Host);
		m_gssPort = ot::json::getString(_jsonObject, OT_ACTION_PARAM_SERVICE_GSS_Port);
	}
}

bool LoginTicketData::isValid() const
{
	return !m_gssHost.empty() && !m_gssPort.empty() && !m_loginTicket.empty();
}