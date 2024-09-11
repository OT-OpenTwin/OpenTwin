//! @file VersionGraphManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/VersionGraphManager.h"

// Qt header
#include <QtWidgets/qlayout.h>

#define OT_VERSIONGRAPHMANAGER_AllMode "View All"
#define OT_VERSIONGRAPHMANAGER_CompactMode "Compact View"
#define OT_VERSIONGRAPHMANAGER_LabelMode "View Labeled Only"

ot::VersionGraphManager::VersionGraphManager() {
	m_root = new QWidget;
	QVBoxLayout* rootLay = new QVBoxLayout(m_root);
	QHBoxLayout* modeLay = new QHBoxLayout;
	
	Label* modeLabel = new Label("Mode:");
	m_modeSelector = new ComboBox;
	m_modeSelector->addItems({
		OT_VERSIONGRAPHMANAGER_AllMode,
		OT_VERSIONGRAPHMANAGER_CompactMode,
		OT_VERSIONGRAPHMANAGER_LabelMode
	});
	m_modeSelector->setCurrentText(OT_VERSIONGRAPHMANAGER_AllMode);

	m_graph = new VersionGraph;

	rootLay->addLayout(modeLay);
	rootLay->addWidget(m_graph, 1);

	modeLay->setContentsMargins(0, 0, 0, 0);
	modeLay->addStretch(1);
	modeLay->addWidget(modeLabel);
	modeLay->addWidget(m_modeSelector);

	this->connect(m_modeSelector, &ComboBox::currentTextChanged, this, &VersionGraphManager::updateCurrentGraph);
}

ot::VersionGraphManager::~VersionGraphManager() {

}

void ot::VersionGraphManager::setupConfig(const VersionGraphCfg& _config) {
	m_config = _config;
	this->updateCurrentGraph();
}

void ot::VersionGraphManager::updateCurrentGraph(void) {
	m_graph->clear();

	if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_AllMode) m_graph->setupFromConfig(m_config);
	else if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_CompactMode) this->updateCurrentGraphCompactMode();
	else if (m_modeSelector->currentText() == OT_VERSIONGRAPHMANAGER_LabelMode) this->updateCurrentGraphLabelMode();
	else {
		OT_LOG_EAS("Invalid mode selected \"" + m_modeSelector->currentText().toStdString() + "\"");
		m_graph->setupFromConfig(m_config);
	}
}

void ot::VersionGraphManager::updateCurrentGraphCompactMode(void) {

}

void ot::VersionGraphManager::updateCurrentGraphLabelMode(void) {

}
