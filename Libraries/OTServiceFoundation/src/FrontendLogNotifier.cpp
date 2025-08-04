//! @file ErrorWarningLogFrontendNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/FrontendLogNotifier.h"

ot::FrontendLogNotifier::FrontendLogNotifier(ApplicationBase* _app)
	: m_app(_app), m_flags(ERROR_LOG | WARNING_LOG | TEST_LOG)
{
	OTAssertNullptr(m_app);
	ot::LogDispatcher::instance().addReceiver(this);
}

ot::FrontendLogNotifier::~FrontendLogNotifier() {

}

void ot::FrontendLogNotifier::log(const LogMessage& _message) {
	if (_message.getFlags() & m_flags) {
		if (m_app->isUiConnected()) {
			OTAssertNullptr(m_app->uiComponent());
			m_app->uiComponent()->displayLogMessage(_message);
		}
	}
}
