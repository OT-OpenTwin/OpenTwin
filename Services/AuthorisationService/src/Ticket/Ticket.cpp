// @otlicense

// Auth header
#include "Ticket/Ticket.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Static functions

std::string Ticket::toString(TicketType _ticketType)
{
	switch (_ticketType)
	{
	case Ticket::Login: return OT_TICKET_TYPE_Login;
	case Ticket::Gateway: return OT_TICKET_TYPE_Gateway;
	case Ticket::Invalid:
		OTAssert(0, "Invalid ticket type should not be serialized.");
		OT_LOG_W("Invalid ticket type should not be serialized.");
		return "Invalid";
	default:
		OT_LOG_ES("Unknown ticket type (" << static_cast<uint32_t>(_ticketType) << ")");
		return "Invalid";
	}
}

Ticket::TicketType Ticket::stringToTicketType(const std::string& _ticketTypeString)
{
	if (_ticketTypeString == OT_TICKET_TYPE_Gateway)
	{
		return TicketType::Gateway;
	}
	else if (_ticketTypeString == OT_TICKET_TYPE_Login)
	{
		return TicketType::Login;
	}
	else
	{
		OT_LOG_E("Unknown ticket type \"" + _ticketTypeString + "\"");
	}

	return TicketType::Invalid;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

Ticket::Ticket(TicketType _type, const std::string& _ticket)
	: m_type(_type)
	, m_ticket(_ticket)
{}