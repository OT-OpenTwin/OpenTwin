// @otlicense

// Auth header
#include "Ticket/Ticket.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"
#include "OTCrypto/CryptoDefaults.h"
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

Ticket::Ticket()
	: m_type(TicketType::Invalid)
	, m_expirationTime(0)
{}

Ticket::Ticket(TicketType _type, uint32_t _validityDurationSeconds, const std::string& _userName)
	: m_type(_type)
	, m_userName(_userName)
	, m_expirationTime(ot::DateTime::msSinceEpoch() + static_cast<int64_t>((_validityDurationSeconds * 1000)))
{
	m_ticket = ot::String::toHexString(ot::RandomNumberProvider::createRandomBytes(ot::CryptoDefaults::ticketLength()));
}

bool Ticket::isValid(const std::string & _ticket, TicketType _ticketType) const
{
	return m_ticket == _ticket
		&& m_type == _ticketType
		&& ot::DateTime::msSinceEpoch() < m_expirationTime;
}
