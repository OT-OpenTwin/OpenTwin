// @otlicense

#pragma once

// Auth header
#include "Ticket/Ticket.h"
#include "Management/User.h"

// std header
#include <map>
#include <unordered_map>

class TicketManager
{
public:
	explicit TicketManager();

	void setMaxTicketCount(Ticket::TicketType _ticketType, uint32_t _maxCount);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Ticket generation

	void clearTicketsForUser(const User& _authenticatedUser);

	//! @brief Generates a ticket of the specified type for the authenticated user and returns the ticket string if successful, otherwise returns std::nullopt.
	std::optional<std::string> generateTicket(const User& _authenticatedUser, Ticket::TicketType _ticketType);

	//! @brief Consumes a ticket of the specified type and returns the corresponding Ticket object if valid, otherwise returns std::nullopt.
	std::optional<Ticket> consumeTicket(Ticket::TicketType _ticketType, const std::string& _ticket, const std::string& _username);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	typedef std::unordered_map<std::string, Ticket> TicketMap;
	typedef std::unordered_map<std::string, TicketMap> UserNameToTicketMap;
	typedef std::unordered_map<std::string, std::string> TicketToUserNameMap;

	UserNameToTicketMap m_tickets;
	TicketToUserNameMap m_ticketToUserMap;
	std::map<Ticket::TicketType, uint32_t> m_ticketTypeToMaxCountMap;
	std::map<Ticket::TicketType, uint32_t> m_ticketTypeToValiditySMap;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Ticket management

	TicketMap& getTicketMapForUser(const User& _authenticatedUser);
	uint32_t countGeneratedTickets(const TicketMap& _ticketMap, Ticket::TicketType _ticketType) const;

};