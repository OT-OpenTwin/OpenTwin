//! @file SceneNodePlot1D.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "SceneNodeBase.h"
#include "OTCore/OTClassHelper.h"

namespace ot { class PlotDataset; }

class SceneNodePlot1DCurve {
	OT_DECL_NOCOPY(SceneNodePlot1DCurve)
	OT_DECL_NODEFAULT(SceneNodePlot1DCurve)
public:
	SceneNodePlot1DCurve(ot::PlotDataset* _dataset);
	virtual ~SceneNodePlot1DCurve();

private:
	ot::PlotDataset* m_dataset;
};
