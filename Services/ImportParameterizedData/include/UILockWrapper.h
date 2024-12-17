#pragma once
#include "OTGui/GuiTypes.h"
#include "Application.h"

class UILockWrapper
{
public:
	UILockWrapper(ot::LockTypeFlag _flag)
		:m_lockFlag(_flag)
	{
		auto uiComponent = Application::instance()->uiComponent();
		if (uiComponent)
		{
			uiComponent->lockUI(m_lockFlag);
		}
		else
		{
			OT_LOG_E("UI component not set");
		}
	}
	~UILockWrapper()
	{
		auto uiComponent = Application::instance()->uiComponent();
		if (uiComponent)
		{
			uiComponent->unlockUI(m_lockFlag);
		}
		else
		{
			OT_LOG_E("UI component not set");
		}
	}
private:
	ot::LockTypeFlag m_lockFlag;
};

