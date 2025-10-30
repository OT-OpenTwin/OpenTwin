// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsVBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
		OT_DECL_NOMOVE(GraphicsVBoxLayoutItemCfg)
		OT_DECL_NOCOPY(GraphicsVBoxLayoutItemCfg)
	public:
		GraphicsVBoxLayoutItemCfg();
		virtual ~GraphicsVBoxLayoutItemCfg() {};
	};

}