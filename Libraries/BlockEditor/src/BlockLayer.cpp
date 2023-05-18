//! @file BlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"
#include "OTBlockEditor/DefaultBlock.h"
#include "OTBlockEditor/BlockConnectorManager.h"

ot::BlockLayer::BlockLayer(ot::DefaultBlock* _block, BlockConnectorManager* _connectorManager) : m_block(_block), m_connectorManager(_connectorManager) {

};

ot::BlockLayer::~BlockLayer() {
	if (m_connectorManager) delete m_connectorManager;
}

ot::BlockLayer::QueueResultFlags ot::BlockLayer::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {
	paintLayer(_arg->rect(), _arg->painter(), _arg->styleOption(), _arg->widget());
	//ToDo: Check if the connector needs to be painted manually
	//if (m_connectorManager) _queue->queue(m_connectorManager);
	return Ok | NoMemClear;
}

QSizeF ot::BlockLayer::layerSizeHint(void) const {
	QSizeF defaultSize(60., 40.);
	adjustSizeToLimits(defaultSize);
	return defaultSize;
}

void ot::BlockLayer::setMargins(const ot::MarginsD& _margins) {
	m_margins.setBottom(_margins.bottom());
	m_margins.setLeft(_margins.left());
	m_margins.setRight(_margins.right());
	m_margins.setTop(_margins.top());
}

void ot::BlockLayer::setConnectorManger(BlockConnectorManager* _connectorManager) {
	if (m_connectorManager) delete m_connectorManager;
	m_connectorManager = _connectorManager;
}

void ot::BlockLayer::adjustSizeToLimits(QSizeF& _size) const {
	// Width min
	if (m_widthLimit.isMinSet()) {
		if (m_widthLimit.min() > _size.width()) {
			_size.setWidth(m_widthLimit.min());
		}
	}

	// Width max
	if (m_widthLimit.isMaxSet()) {
		if (m_widthLimit.max() < _size.width()) {
			_size.setWidth(m_widthLimit.max());
		}
	}

	// Height min
	if (m_heightLimit.isMinSet()) {
		if (m_heightLimit.min() > _size.height()) {
			_size.setHeight(m_heightLimit.min());
		}
	}

	// Height max
	if (m_heightLimit.isMaxSet()) {
		if (m_heightLimit.max() < _size.height()) {
			_size.setHeight(m_heightLimit.max());
		}
	}
}