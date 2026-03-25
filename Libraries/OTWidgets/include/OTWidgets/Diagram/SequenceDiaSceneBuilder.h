// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Diagram/SequenceDiaFunctionCfg.h"
#include "OTWidgets/WidgetTypes.h"

// std header
#include <map>

namespace ot {

	class GraphicsView;
	class SequenceDiaLifeLine;

	class OT_WIDGETS_API_EXPORT SequenceDiaSceneBuilder
	{
		OT_DECL_NOCOPY(SequenceDiaSceneBuilder)
		OT_DECL_NOMOVE(SequenceDiaSceneBuilder)
		OT_DECL_NODEFAULT(SequenceDiaSceneBuilder)
	public:
		
		SequenceDiaSceneBuilder(GraphicsView* _graphicsView);
		virtual ~SequenceDiaSceneBuilder();

		void build();

		void addFunction(const SequenceDiaFunctionCfg& _functionCfg);

	private:
		bool m_needsBuild = true;
		GraphicsView* m_view = nullptr;
		std::list<SequenceDiaLifeLine*> m_lifeLines;

	};

}