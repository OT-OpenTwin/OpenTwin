#pragma once

// OpenTwin header
#include "OTGui/VisualisationTypes.h"

class SceneNodeBase;

class VisualiserHelper {
	OT_DECL_STATICONLY(VisualiserHelper)
public:
	static void addVisualizer(SceneNodeBase* _node, const ot::VisualisationTypes& _types);
};