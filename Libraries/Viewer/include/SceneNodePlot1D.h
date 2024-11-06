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
namespace ot { class PlotDataset; };

class SceneNodePlot1D : public SceneNodeBase {
	OT_DECL_NOCOPY(SceneNodePlot1D)
public:
	SceneNodePlot1D();
	virtual ~SceneNodePlot1D();

	void setConfig(const ot::Plot1DCfg& _config);
	void setFromDataBaseConfig(const ot::Plot1DDataBaseCfg& _config);
	const ot::Plot1DCfg& getConfig(void);

	virtual bool isItem3D(void) override { return false; };

	std::list<ot::PlotDataset*> getDatasets(void) const;
	bool changeCurveEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion);

	void setIncompatibleData(void);
	void removeErrorState(void);

private:
	bool isAnyChildSelectedAndVisible(SceneNodeBase* _root);

	ot::PlotManager* m_plot;
};
