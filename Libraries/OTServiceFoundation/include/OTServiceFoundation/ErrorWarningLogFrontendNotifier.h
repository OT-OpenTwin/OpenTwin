//! @file ErrorWarningLogFrontendNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTClassHelper.h"

namespace ot {

	class ApplicationBase;

	//! \brief Forwards error and warning logs to the frontend if one is connected.
	class ErrorWarningLogFrontendNotifier : public AbstractLogNotifier {
		OT_DECL_NOCOPY(ErrorWarningLogFrontendNotifier)
		OT_DECL_NODEFAULT(ErrorWarningLogFrontendNotifier)
	public:
		//! \brief Will register at the LogDispatcher.
		//! \note The LogDispatcher will take the ownership upon object creation.
		ErrorWarningLogFrontendNotifier(ApplicationBase* _app);
		virtual ~ErrorWarningLogFrontendNotifier();

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;

	private:
		ApplicationBase* m_app;

	};

}