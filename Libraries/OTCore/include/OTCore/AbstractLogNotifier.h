//! @file AbstractLogNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogMessage.h"

namespace ot {

	//! @brief Used to receive every log message that is generated.
	class OT_CORE_API_EXPORTONLY AbstractLogNotifier {
	public:
		AbstractLogNotifier() : m_customDelete(false) {};
		virtual ~AbstractLogNotifier() {};

		//! @brief Will set the delete later flag.
		//! If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
		void setCustomDeleteLogNotifier(bool _customDelete = true) { m_customDelete = _customDelete; };

		//! @brief Returns true if the delete later mode is set.
		bool getCustomDeleteLogNotifier() const { return m_customDelete; };

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) = 0;

	private:
		bool m_customDelete; //! @brief If delete later is set, the creator keeps ownership of this object even after it is added to the LogDispatcher.
	};

}