//! @file BlockNetworkEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockNetworkEditor.h"
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockFactory.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockConfigurationFactory.h"
#include "OpenTwinCore/Logger.h"

// Qt header
#include <QtGui/qevent.h>

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::BlockNetworkEditor::BlockNetworkEditor() {
	m_network = new BlockNetwork;
	setScene(m_network);

	setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

ot::BlockNetworkEditor::~BlockNetworkEditor() {
	delete m_network;
}

// ########################################################################################################

void ot::BlockNetworkEditor::mousePressedMoveEvent(QMouseEvent* _event) {
	m_network->update();
}

void ot::BlockNetworkEditor::dragEnterEvent(QDragEnterEvent* _event) {
	OT_LOG_D("BlockNetworkEditor: Drag Enter");

	// Check if the events mime data contains the configuration
	if (!_event->mimeData()->data(OT_BLOCK_MIMETYPE_Configuration).isEmpty()) {
		_event->acceptProposedAction();
	}
}

void ot::BlockNetworkEditor::dropEvent(QDropEvent* _event) {
	OT_LOG_D("BlockNetworkEditor: Drop");

	QByteArray cfgRaw = _event->mimeData()->data(OT_BLOCK_MIMETYPE_Configuration);
	if (cfgRaw.isEmpty()) {
		return;
	}
	// Generate configuration from raw data
	ot::BlockConfiguration* cfg = nullptr;
	try {
		OT_rJSON_parseDOC(cfgDoc, cfgRaw.toStdString().c_str());
		OT_rJSON_val cfgObj = cfgDoc.GetObject();
		cfg = ot::BlockConfigurationFactory::blockConfigurationFromJson(cfgObj);
	}
	catch (const std::exception& e) {
		OT_LOG_EAS(e.what());
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
	}

	if (cfg == nullptr) {
		OT_LOG_WA("No config created");
		return;
	}

	// Store current event position to position the new block at this pos
	QPointF position = mapToScene(mapFromGlobal(_event->pos()));

	Block* newBlock = ot::BlockFactory::blockFromConfig(cfg);
	newBlock->setPos(position);
	m_network->addItem(newBlock);

	_event->acceptProposedAction();
}