//! @file ErrorWarningLogFrontendNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ErrorWarningLogFrontendNotifier.h"

ot::ErrorWarningLogFrontendNotifier::ErrorWarningLogFrontendNotifier(ApplicationBase* _app)
	: m_app(_app)
{
	OTAssertNullptr(m_app);
	ot::LogDispatcher::instance().addReceiver(this);
}

ot::ErrorWarningLogFrontendNotifier::~ErrorWarningLogFrontendNotifier() {

}

void ot::ErrorWarningLogFrontendNotifier::log(const LogMessage& _message) {
	static const ot::LogFlag flags = ot::ERROR_LOG | ot::WARNING_LOG;
	if (_message.getFlags() & flags) {
		if (m_app->isUiConnected()) {
			OTAssertNullptr(m_app->uiComponent());
			
			StyledTextBuilder message;

			message << "[";
			if (_message.getFlags() & ot::ERROR_LOG) {
				message << StyledText::Error << StyledText::Bold << "ERROR" << StyledText::ClearStyle;
			}
			else if (_message.getFlags() & ot::WARNING_LOG) {
				message << StyledText::Warning << StyledText::Bold << "WARNING" << StyledText::ClearStyle;
			}
			message << "] [" << _message.getServiceName() << "] " << _message.getText();
			
			// Display
			m_app->uiComponent()->displayStyledMessage(message);
		}
	}
}
