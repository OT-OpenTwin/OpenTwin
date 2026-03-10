// @otlicese

#pragma once

// OpenTwin header
#include "OTGuiAPI/ContextMenuHandler.h"
#include "OTGui/Event/NavigationContextRequestData.h"

class Application;

class ContextActionHandler : public ot::ContextMenuHandler
{
	OT_DECL_NOCOPY(ContextActionHandler)
	OT_DECL_NOMOVE(ContextActionHandler)
	OT_DECL_NODEFAULT(ContextActionHandler)
public:
	ContextActionHandler(Application* _application);
	~ContextActionHandler() = default;

protected:

	OT_DECL_NODISCARD virtual ot::MenuCfg contextMenuRequested(const ot::ContextMenuRequestEvent& _event) override;

private:
	ot::MenuCfg createNavigationContextMenu(const ot::NavigationContextRequestData& _requestData) const;

	Application* m_app;

};