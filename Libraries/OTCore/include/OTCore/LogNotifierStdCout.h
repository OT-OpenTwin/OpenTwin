// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/AbstractLogNotifier.h"

namespace ot {

	//! @brief Used to write created log messages to std cout in a way a human could read it.
	class OT_CORE_API_EXPORT LogNotifierStdCout : public AbstractLogNotifier {
	public:
		virtual ~LogNotifierStdCout() {};

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;
	};

}