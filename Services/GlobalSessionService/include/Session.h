//! @file Session.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

//! @class Session
//! @brief The Session class contains all relevant information about a session.
class Session : public ot::Serializable {
public:
	//! @brief State flag describing the current state of the session.
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

	// Serialization

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setStateFlag(SessionStateFlag _flag, bool _active = true) { m_state.setFlag(_flag, _active); };
	void setState(const SessionState& _state) { m_state = _state; };
	const SessionState& getState(void) const { return m_state; };

	//! @brief Set the session id.
	//! @param _id ID to set.
	void setId(const std::string& _id) { m_id = _id; };

	//! @brief Returns the session id.
	const std::string& getId(void) const { return m_id; };

	//! @brief Set the session user name.
	//! @param _name User name to set.
	void setUserName(const std::string& _name) { m_userName = _name; };

	//! @brief Returns the session user name.
	const std::string& getUserName(void) const { return m_userName; };

private:
	SessionState m_state;   //! @brief Session state flags.
	std::string m_id;       //! @brief Session id.
	std::string m_userName; //! @brief User name.
};

OT_ADD_FLAG_FUNCTIONS(Session::SessionStateFlag)