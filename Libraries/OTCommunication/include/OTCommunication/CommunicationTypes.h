#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"		// Add flag fuctions
#include "OTCore/Logger.h"	// LogFlag type

namespace ot {

	//! @brief The message types describes how a message should be delivered
	enum MessageType {
		QUEUE							= 0x01,
		EXECUTE							= 0x02,
		EXECUTE_ONE_WAY_TLS				= 0x04,
		SECURE_MESSAGE_TYPES			= 0x03,
		ALL_MESSAGE_TYPES				= 0xFF
	};

	//! @brief The message types describes how a message should be delivered
	typedef ot::Flags<ot::MessageType> MessageTypeFlags;

	__declspec(dllexport) ot::LogFlag messageTypeToLogFlag(ot::MessageType _type);

	//! @brief Returns a string representation for the given message type flag
	//! @param _flag The message type flag
	__declspec(dllexport) std::string toString(MessageType _flag);

	//! @brief Returns the message type that is represented by the provided string
	//! @param _flag The string containing the message type flag
	__declspec(dllexport) MessageType stringToMessageTypeFlag(const std::string& _type);
}

OT_ADD_FLAG_FUNCTIONS(ot::MessageType);