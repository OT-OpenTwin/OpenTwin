// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsHBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
		OT_DECL_NOCOPY(GraphicsHBoxLayoutItemCfg)
		OT_DECL_NOMOVE(GraphicsHBoxLayoutItemCfg)
	public:
		GraphicsHBoxLayoutItemCfg();
		virtual ~GraphicsHBoxLayoutItemCfg() {};
	};

}