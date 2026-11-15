// @otlicense

#pragma once

// Auth header
#include "Ticket/Ticket.h"
#include "Management/User.h"

// std header
#include <unordered_map>

class TicketManager
{
public:
	TicketManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Ticket generation

	std::optional<std::string> generateTicket(const User& _authenticatedUser, Ticket::TicketType _ticketType);
	std::optional<std::string> generateTicket(const User& _authenticatedUser, const std::string& _ticketTypeString);

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	std::unordered_map<std::string, Ticket::TicketType> m_tickets;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Ticket generation

	std::string generateNewTicketString();
	std::optional<std::string> generateLoginTicket(const User& _authenticatedUser);
	std::optional<std::string> generateGatewayTicket(const User& _authenticatedUser);

};