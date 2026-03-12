// @otlicese

#pragma once

// OpenTwin header
#include "OTGuiAPI/ContextMenuHandler.h"

namespace ot {
	class NavigationContextRequestData;
}

class ContextActionHandler : public ot::ContextMenuHandler
{
	OT_DECL_NOCOPY(ContextActionHandler)
	OT_DECL_NOMOVE(ContextActionHandler)
public:
	ContextActionHandler();
	~ContextActionHandler();

protected:

	OT_DECL_NODISCARD virtual ot::MenuCfg contextMenuRequested(const ot::ContextMenuRequestEvent& _event) override;

private:
	ot::MenuCfg createNavigationContextMenu(const ot::NavigationContextRequestData* _requestData) const;

	class PrivateData;
	PrivateData* m_data;
};