// @otlicese

#pragma once

// OpenTwin header
#include "OTGuiAPI/MenuHandler.h"

namespace ot {
	class NavigationMenuRequestData;
}

class ContextActionHandler : public ot::MenuHandler
{
	OT_DECL_NOCOPY(ContextActionHandler)
	OT_DECL_NOMOVE(ContextActionHandler)
public:
	ContextActionHandler();
	~ContextActionHandler();

protected:

	OT_DECL_NODISCARD virtual ot::MenuCfg menuRequested(const ot::MenuRequestEvent& _event) override;

private:
	ot::MenuCfg createNavigationContextMenu(const ot::NavigationMenuRequestData* _requestData) const;

	class PrivateData;
	PrivateData* m_data;
};