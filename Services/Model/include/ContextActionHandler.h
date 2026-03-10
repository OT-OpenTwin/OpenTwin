// @otlicese

#pragma once

// OpenTwin header
#include "OTGuiAPI/ContextMenuHandler.h"
#include "OTGui/Event/NavigationContextRequestData.h"

class ContextActionHandler : public ot::ContextMenuHandler
{
public:
	ContextActionHandler() = default;
	~ContextActionHandler() = default;

protected:

	OT_DECL_NODISCARD virtual ot::MenuCfg contextMenuRequested(const ot::ContextMenuRequestEvent& _event) override;

private:
	ot::MenuCfg createNavigationContextMenu(const ot::NavigationContextRequestData& _requestData) const;

};