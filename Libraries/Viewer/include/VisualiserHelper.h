#pragma once

// OpenTwin header
#include "OTGui/VisualisationTypes.h"

#include "TextVisualiser.h"
#include "TableVisualiser.h"
#include "PlotVisualiser.h"
#include "CurveVisualiser.h"
#include "RangeVisualiser.h"
#include "GraphicsVisualiser.h"
#include "GraphicsItemVisualiser.h"
#include "GraphicsConnectionVisualiser.h"

class SceneNodeBase;

class VisualiserHelper {
	OT_DECL_STATICONLY(VisualiserHelper)
public:
	static void addVisualizer(SceneNodeBase* _node, const ot::VisualisationTypes& _types);
};