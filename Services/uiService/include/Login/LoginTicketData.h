// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

class LoginTicketData : public ot::Serializable
{
	OT_DECL_DEFCOPY(LoginTicketData)
	OT_DECL_DEFMOVE(LoginTicketData)
public:
	explicit LoginTicketData() = default;
	~LoginTicketData() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// JSON serialization

	virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setGssHost(const std::string& _host) { m_gssHost = _host; };
	const std::string& getGssHost() const { return m_gssHost; };

	void setGssPort(const std::string& _port) { m_gssPort = _port; };
	const std::string& getGssPort() const { return m_gssPort; };

	void setLoginTicket(const std::string& _ticket) { m_loginTicket = _ticket; };
	const std::string& getLoginTicket() const { return m_loginTicket; };

	bool isValid() const;

private:
	std::string m_gssHost;
	std::string m_gssPort;
	std::string m_loginTicket;
};