//! @file SceneNodePlot1D.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "SceneNodeBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"

namespace ot { class PlotDataset; }

class SceneNodePlot1DCurve : public SceneNodeBase {
	OT_DECL_NOCOPY(SceneNodePlot1DCurve)
	OT_DECL_NODEFAULT(SceneNodePlot1DCurve)
public:
	SceneNodePlot1DCurve(ot::PlotDataset* _dataset);
	virtual ~SceneNodePlot1DCurve();

	virtual bool isItem3D(void) override { return false; };

	void setModelEntityVersion(ot::UID _version) { m_modelEntityVersion = _version; };
	ot::UID getModelEntityVersion(void) const { return m_modelEntityVersion; };

	ot::PlotDataset* getDataset(void) { return m_dataset; };

private:
	ot::UID m_modelEntityVersion;
	ot::PlotDataset* m_dataset;
};
