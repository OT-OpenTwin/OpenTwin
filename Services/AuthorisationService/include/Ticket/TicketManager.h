// @otlicense

#pragma once

// Auth header
#include "Management/User.h"

class TicketManager
{
public:
	enum TicketType : uint32_t
	{
		Invalid,
		Login,
		Gateway
	};

	static std::string toString(TicketType _ticketType);
	static TicketType stringToTicketType(const std::string& _ticketTypeString);

	TicketManager();

	std::optional<std::string> generateTicket(const User& _authenticatedUser, TicketType _ticketType);

private:
	std::string generateNewTicketString();

};