// @otlicense

// Auth header
#include "Ticket/TicketManager.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/Msg.h"

TicketManager::TicketManager()
	: m_ticketTypeToMaxCountMap({
		{ Ticket::Login, 10 },
		{ Ticket::Gateway, 50 }
		})
	, m_ticketTypeToValiditySMap({ 
		{ Ticket::Login, ot::msg::defaultTimeout * 10 }, 
		{ Ticket::Gateway, std::max((24 * 3600) - 1, ot::msg::defaultTimeout * 1000) } 
		})
{}

void TicketManager::setMaxTicketCount(Ticket::TicketType _ticketType, uint32_t _maxCount)
{
	m_ticketTypeToMaxCountMap[_ticketType] = _maxCount;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Ticket generation

void TicketManager::clearTicketsForUser(const User& _authenticatedUser)
{
	auto& ticketsMap = getTicketMapForUser(_authenticatedUser);
	for (const auto& [ticketID, ticket] : ticketsMap)
	{
		m_ticketToUserMap.erase(ticketID);
	}

	m_tickets.erase(_authenticatedUser.getID());
}

std::optional<std::string> TicketManager::generateTicket(const User & _authenticatedUser, Ticket::TicketType _ticketType)
{
	// Ensure that the ticket type is valid
	if (_ticketType == Ticket::Invalid)
	{
		OT_LOG_E("Invalid ticket type specified.");
		return std::nullopt;
	}

	// Get the ticket map for the authenticated user
	auto& ticketMap = getTicketMapForUser(_authenticatedUser);

	// Check if the maximum number of tickets for this type has been reached
	OTAssert(m_ticketTypeToMaxCountMap.find(_ticketType) != m_ticketTypeToMaxCountMap.end(), "Ticket type not found in max count map.");
	uint32_t maxTicketCount = m_ticketTypeToMaxCountMap[_ticketType];

	if (countGeneratedTickets(ticketMap, _ticketType) >= maxTicketCount)
	{
		OT_LOG_WS("Maximum number of tickets (" << maxTicketCount << ") for type \"" << Ticket::toString(_ticketType) << "\" reached for user \"" << _authenticatedUser.getName() << "\".");
		return std::nullopt;
	}

	// Generate a new ticket
	OTAssert(m_ticketTypeToValiditySMap.find(_ticketType) != m_ticketTypeToValiditySMap.end(), "Ticket type not found in validity duration map.");
	auto validityDurationS = m_ticketTypeToValiditySMap[_ticketType];

	Ticket ticket(_ticketType, validityDurationS, _authenticatedUser.getName());

	// Store the ticket in the user's ticket map and update the ticket-to-user map
	m_ticketToUserMap[ticket.getTicket()] = _authenticatedUser.getID();
	auto result = ticketMap.insert_or_assign(ticket.getTicket(), std::move(ticket));
	return result.first->second.getTicket();
}

std::optional<Ticket> TicketManager::consumeTicket(Ticket::TicketType _ticketType, const std::string& _ticket)
{
	// Find the user associated with the ticket
	auto userIt = m_ticketToUserMap.find(_ticket);
	if (userIt == m_ticketToUserMap.end())
	{
		OT_LOG_D("Ticket \"" + _ticket + "\" not found.");
		return std::nullopt;
	}

	// Find the ticket map for the user
	auto ticketMapIt = m_tickets.find(userIt->second);
	if (ticketMapIt == m_tickets.end())
	{
		OT_LOG_E("Ticket \"" + _ticket + "\" found in ticket-to-user map, but user \"" + userIt->second + "\" has no tickets.");
		return std::nullopt;
	}

	// Find the ticket in the user's ticket map
	auto ticketIt = ticketMapIt->second.find(_ticket);
	if (ticketIt == ticketMapIt->second.end())
	{
		OT_LOG_E("Ticket \"" + _ticket + "\" found in ticket-to-user map, but not in the user's ticket map.");
		return std::nullopt;
	}

	// Move the ticket out of the map and erase it
	std::optional<Ticket> ticket = std::move(ticketIt->second);
	ticketMapIt->second.erase(ticketIt);

	// Remove the ticket from the ticket-to-user map
	m_ticketToUserMap.erase(_ticket);

	// Validate the ticket
	if (!ticket->isValid(_ticket, _ticketType))
	{
		OT_LOG_W("Ticket \"" + _ticket + "\" is not valid.");
		return std::nullopt;
	}
	else
	{
		return ticket;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Ticket management

TicketManager::TicketIDToTicketMap& TicketManager::getTicketMapForUser(const User& _authenticatedUser)
{
	const std::string& userID = _authenticatedUser.getID();
	auto it = m_tickets.find(userID);
	if (it == m_tickets.end())
	{
		it = m_tickets.emplace(userID, TicketIDToTicketMap()).first;
	}
	return it->second;
}

uint32_t TicketManager::countGeneratedTickets(const TicketIDToTicketMap& _ticketMap, Ticket::TicketType _ticketType) const
{
	uint32_t count = 0;

	for (const auto& [ticketID, ticket] : _ticketMap)
	{
		if (ticket.getType() == _ticketType)
		{
			count++;
		}
	}

	return count;
}
