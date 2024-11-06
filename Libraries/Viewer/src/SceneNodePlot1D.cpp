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

void SceneNodePlot1D::setConfig(const ot::Plot1DCfg& _config) {
	m_plot->setConfig(_config);
}

void SceneNodePlot1D::setFromDataBaseConfig(const ot::Plot1DDataBaseCfg& _config) {
	m_plot->setFromDataBaseConfig(_config);
}

const ot::Plot1DCfg& SceneNodePlot1D::getConfig(void) {
	return m_plot->getConfig();
}

std::list<ot::PlotDataset*> SceneNodePlot1D::getDatasets(void) const {
	return m_plot->getDatasets();
}

bool SceneNodePlot1D::changeCurveEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion) {
	return (m_plot->changeCachedDatasetEntityVersion(_curveEntityID, _curveVersion) && this->isAnyChildSelectedAndVisible(this));
}

void SceneNodePlot1D::setIncompatibleData(void) {
	m_plot->setIncompatibleData();
}

void SceneNodePlot1D::removeErrorState(void) {
	m_plot->setErrorState(false);
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