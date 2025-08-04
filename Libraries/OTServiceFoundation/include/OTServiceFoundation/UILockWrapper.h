#pragma once
#pragma once
#include "OTGui/GuiTypes.h"
#include "OTServiceFoundation/UiComponent.h"

class __declspec(dllexport) UILockWrapper
{
public:
	UILockWrapper(ot::components::UiComponent* _uiComponent,ot::LockTypeFlag _flag)
		:m_lockFlag(_flag), m_uiComponent(_uiComponent)
	{
		if (m_uiComponent)
		{
			OT_LOG_D("Applying UI lock.");
			m_uiComponent->lockUI(m_lockFlag);
		}
	}
	~UILockWrapper()
	{
		if (m_uiComponent)
		{
			OT_LOG_D("Release UI lock.");
			m_uiComponent->unlockUI(m_lockFlag);
		}
	}
private:
	ot::LockTypeFlag m_lockFlag;
	ot::components::UiComponent* m_uiComponent = nullptr;
};

