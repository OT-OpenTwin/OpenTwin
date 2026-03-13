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
	ContextActionHandler();
	~ContextActionHandler();

protected:

	OT_DECL_NODISCARD virtual ot::MenuCfg menuRequested(const ot::MenuRequestEvent& _event) override;

private:
	ot::MenuCfg createContextMenu(const ot::NavigationMenuRequestData* _requestData) const;
	ot::MenuCfg createContextMenu(const ot::GraphicsMenuRequestData* _requestData) const;
	ot::MenuCfg createContextMenu(const ot::PlotMenuRequestData* _requestData) const;
	ot::MenuCfg createContextMenu(const ot::TableMenuRequestData* _requestData) const;
	ot::MenuCfg createContextMenu(const ot::TextEditorMenuRequestData* _requestData) const;
	ot::MenuCfg createContextMenu(const ot::View3DMenuRequestData* _requestData) const;

	class PrivateData;
	PrivateData* m_data;
};