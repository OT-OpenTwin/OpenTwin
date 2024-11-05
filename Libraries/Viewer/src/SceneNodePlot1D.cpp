//! @file SceneNodePlot1D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotManager.h"
#include "SceneNodePlot1D.h"
#include "OTWidgets/PlotDataset.h"

SceneNodePlot1D::SceneNodePlot1D() {
	m_plot = new ot::PlotManager;
}

SceneNodePlot1D::~SceneNodePlot1D() {
	delete m_plot;
}

bool SceneNodePlot1D::changeResult1DEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion) {
	return (m_plot->changeCachedDatasetEntityVersion(_curveEntityID, _curveVersion) && this->isAnyChildSelectedAndVisible(this));
}

bool SceneNodePlot1D::isAnyChildSelectedAndVisible(SceneNodeBase* _root) {
	if (_root->isSelected() && _root->isVisible()) {
		return true;
	}

	for (auto child : _root->getChildren()) {
		if (this->isAnyChildSelectedAndVisible(child)) {
			return true;
		}
	}

	return false;
}