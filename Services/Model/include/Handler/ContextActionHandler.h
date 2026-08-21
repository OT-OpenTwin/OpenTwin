// @otlicese

#pragma once

// OpenTwin header
#include "OTGuiAPI/MenuHandler.h"

namespace ot {
	class NavigationMenuRequestData;
	class GraphicsMenuRequestData;
	class PlotMenuRequestData;
	class TableMenuRequestData;
	class TextEditorMenuRequestData;
	class View3DMenuRequestData;
}

class ContextActionHandler : public ot::MenuHandler
{
	OT_DECL_NOCOPY(ContextActionHandler)
	OT_DECL_NOMOVE(ContextActionHandler)
public:
	ContextActionHandler() = default;
	virtual ~ContextActionHandler() = default;

protected:
	OT_DECL_NODISCARD virtual ot::MenuCfg menuRequested(const ot::MenuRequestEvent& _event) override;

};