// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <string>

class Ticket
{
	OT_DECL_NOCOPY(Ticket)
	OT_DECL_DEFMOVE(Ticket)
	OT_DECL_NODEFAULT(Ticket)
public:
	enum TicketType : uint32_t
	{
		Invalid,
		Login,
		Gateway
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Static functions

	static std::string toString(TicketType _ticketType);
	static TicketType stringToTicketType(const std::string& _ticketTypeString);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor / Destructor

	Ticket(TicketType _type, const std::string& _ticket);
	~Ticket() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	TicketType getType() const { return m_type; };
	std::string getTicket() const { return m_ticket; };

private:
	TicketType m_type;
	std::string m_ticket;

};