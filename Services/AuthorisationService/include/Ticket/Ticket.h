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

	explicit Ticket();
	explicit Ticket(TicketType _type, uint32_t _validityDurationSeconds, const std::string& _userName);
	~Ticket() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Validation
	
	const std::string& getTicket() const { return m_ticket; };
	TicketType getType() const { return m_type; };
	const std::string& getUsername() const { return m_userName; };
	bool isValid(const std::string& _ticket, TicketType _ticketType) const;

private:
	int64_t m_expirationTime;
	TicketType m_type;
	std::string m_userName;
	std::string m_ticket;

};