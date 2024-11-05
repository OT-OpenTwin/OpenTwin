//! @file SceneNodePlot1D.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "SceneNodeBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"

namespace ot { class PlotManager; };

class SceneNodePlot1D : public SceneNodeBase {
	OT_DECL_NOCOPY(SceneNodePlot1D)
public:
	SceneNodePlot1D();
	virtual ~SceneNodePlot1D();

	bool changeResult1DEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion);

private:
	bool isAnyChildSelectedAndVisible(SceneNodeBase* _root);

	ot::PlotManager* m_plot;
};
