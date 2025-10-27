// @otlicense

#pragma once

#include "OTServiceFoundation/UiComponent.h"

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT UILockWrapper {
	public:
		UILockWrapper(ot::components::UiComponent* _uiComponent, const ot::LockTypes& _flags);
		~UILockWrapper();

	private:
		ot::LockTypes m_lockFlags;
		ot::components::UiComponent* m_uiComponent = nullptr;
	};

}