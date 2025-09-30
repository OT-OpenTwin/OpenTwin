//! @file FrontendLogNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/AbstractLogNotifier.h"

namespace ot {

	class ApplicationBase;

	//! \brief Forwards error and warning logs to the frontend if one is connected.
	class FrontendLogNotifier : public AbstractLogNotifier {
		OT_DECL_NOCOPY(FrontendLogNotifier)
		OT_DECL_NODEFAULT(FrontendLogNotifier)
	public:
		//! \brief Will register at the LogDispatcher.
		//! \note The LogDispatcher will take the ownership upon object creation.
		FrontendLogNotifier(ApplicationBase* _app);
		virtual ~FrontendLogNotifier();

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;

		void setFlags(ot::LogFlags _flags) { m_flags = _flags; }
		const ot::LogFlags& getFlags(void) const { return m_flags; }

	private:
		ApplicationBase* m_app;
		ot::LogFlags m_flags;

	};

}