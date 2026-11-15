// @otlicense

// Auth header
#include "Ticket/TicketManager.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCore/String.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"

TicketManager::TicketManager()
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Ticket generation

std::optional<std::string> TicketManager::generateTicket(const User & _authenticatedUser, Ticket::TicketType _ticketType)
{
	switch (_ticketType)
	{
	case Ticket::Invalid:
		return std::nullopt;
	case Ticket::Login:
		break;
	case Ticket::Gateway:
		break;
	default:
		break;
	}

	return std::optional<std::string>();
}



std::optional<std::string> TicketManager::generateTicket(const User& _authenticatedUser, const std::string& _ticketTypeString)
{
	const Ticket::TicketType ticketType = Ticket::stringToTicketType(_ticketTypeString);
	return generateTicket(_authenticatedUser, ticketType);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Ticket generation

std::string TicketManager::generateNewTicketString()
{
	std::vector<uint8_t> ticketData = ot::RandomNumberProvider::createRandomBytes(32);
	return ot::String::toHexString(ticketData);
}
