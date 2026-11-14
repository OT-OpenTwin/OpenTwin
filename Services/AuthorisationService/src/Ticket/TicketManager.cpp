// @otlicense

// Auth header
#include "Ticket/TicketManager.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"

std::string TicketManager::toString(TicketType _ticketType)
{
	switch (_ticketType)
	{
	case TicketManager::Login: return OT_TICKET_TYPE_Login;
	case TicketManager::Gateway: return OT_TICKET_TYPE_Gateway;
	default:
		OT_LOG_ES("Unknown ticket type (" << static_cast<uint32_t>(_ticketType) << ")");
		return "Invalid";
	}
}

TicketManager::TicketType TicketManager::stringToTicketType(const std::string& _ticketTypeString)
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

TicketManager::TicketManager()
{}

std::optional<std::string> TicketManager::generateTicket(const User & _authenticatedUser, TicketType _ticketType)
{
	if (_ticketType == TicketManager::TicketType::Invalid)
	{
		return std::nullopt;
	}

	std::string newTicket = generateNewTicketString();

	switch (_ticketType)
	{
	case TicketManager::Invalid: return std::nullopt;
		break;
	case TicketManager::Login:
		break;
	case TicketManager::Gateway:
		break;
	default:
		break;
	}

	return std::optional<std::string>();
}

std::string TicketManager::generateNewTicketString()
{
	std::vector<uint8_t> ticketData = ot::RandomNumberProvider::createRandomBytes(32);
	return ot::String::toHexString(ticketData);
}
