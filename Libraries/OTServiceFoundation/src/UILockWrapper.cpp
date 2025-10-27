// @otlicense

#include "OTServiceFoundation/UILockWrapper.h"

ot::UILockWrapper::UILockWrapper(ot::components::UiComponent* _uiComponent, const ot::LockTypes& _flags)
	: m_lockFlags(_flags), m_uiComponent(_uiComponent) 
{
	OTAssertNullptr(m_uiComponent);
	if (m_uiComponent) {
		OT_LOG_D("Applying UI lock.");
		m_uiComponent->lockUI(m_lockFlags);
	}
}

ot::UILockWrapper::~UILockWrapper() {
	if (m_uiComponent) {
		OT_LOG_D("Release UI lock.");
		m_uiComponent->unlockUI(m_lockFlags);
	}
}