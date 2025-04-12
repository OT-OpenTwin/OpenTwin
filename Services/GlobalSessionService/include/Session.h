//! @file Session.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

class Session {
public:
	enum SessionStateFlag {
		NoStateFlags  = 0 << 0, //! @brief No state flags are set

		//! @brief The LSS received the create session request.
		//! This is the "default" state.
		//! The session is assumed to be open if the LSS received the request.
		LssConfirmed  = 1 << 0
	};
	typedef ot::Flags<SessionStateFlag> SessionState;

	Session();
	Session(const std::string& _id);
	Session(const Session& _other);
	Session(Session&& _other) noexcept;
	virtual ~Session();

	Session& operator = (const Session& _other);
	Session& operator = (Session&& _other) noexcept;

	bool operator == (const Session& _other) const;
	bool operator != (const Session& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setStateFlag(SessionStateFlag _flag, bool _active = true) { m_state.setFlag(_flag, _active); };
	void setState(const SessionState& _state) { m_state = _state; };
	const SessionState& getState(void) const { return m_state; };

	void setId(const std::string& _id) { m_id = _id; };
	const std::string& getId(void) const { return m_id; };

	void setUserName(const std::string& _name) { m_userName = _name; };
	const std::string& getUserName(void) const { return m_userName; };

private:
	SessionState m_state;
	std::string m_id;
	std::string m_userName;
};

OT_ADD_FLAG_FUNCTIONS(Session::SessionStateFlag)