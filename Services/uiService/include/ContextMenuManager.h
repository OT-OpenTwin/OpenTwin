// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/ContextRequestData.h"

class AppBase;

class ContextMenuManager
{

public:
	ContextMenuManager(AppBase* _app);
	virtual ~ContextMenuManager();

	bool navigationItemContextRequest(const ot::BasicEntityInformation& _hoveredEntity, ot::MenuCfg& _resultMenu);

private:
	AppBase* m_app;
};