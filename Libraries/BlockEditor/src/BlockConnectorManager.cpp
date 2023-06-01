//! @file BlockConnectorManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnectorManager.h"
#include "OTBlockEditor/Block.h"
#include "OpenTwinCore/otAssert.h"

ot::BorderLayoutBlockConnectorManager::BorderLayoutBlockConnectorManager(BlockLayer* _layer) : BlockConnectorManager(_layer) {

}

ot::BorderLayoutBlockConnectorManager::~BorderLayoutBlockConnectorManager() {

}

bool ot::BorderLayoutBlockConnectorManager::addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) {
	OTAssertNullptr(_connector);

	if (_flags.flagIsSet(ot::ConnectorPushFront)) {
		// Front
		if (_flags.flagIsSet(ConnectorTop)) { m_top.push_front(_connector); }
		else if (_flags.flagIsSet(ConnectorRight)) { m_right.push_front(_connector); }
		else if (_flags.flagIsSet(ConnectorBottom)) { m_bottom.push_front(_connector); }
		else if (_flags.flagIsSet(ConnectorLeft)) { m_left.push_front(_connector); }
	}
	else {
		// Back
		if (_flags.flagIsSet(ConnectorTop)) { m_top.push_back(_connector); }
		else if (_flags.flagIsSet(ConnectorRight)) { m_right.push_back(_connector); }
		else if (_flags.flagIsSet(ConnectorBottom)) { m_bottom.push_back(_connector); }
		else if (_flags.flagIsSet(ConnectorLeft)) { m_left.push_back(_connector); }
	}

	return true;
}

std::list<ot::BlockConnector*> ot::BorderLayoutBlockConnectorManager::getAllConnectors(void) const {
	std::list<ot::BlockConnector*> ret;
	for (auto c : m_top) ret.push_back(c);
	for (auto c : m_left) ret.push_back(c);
	for (auto c : m_right) ret.push_back(c);
	for (auto c : m_bottom) ret.push_back(c);
	return ret;
}

ot::BorderLayoutBlockConnectorManager::QueueResultFlags ot::BorderLayoutBlockConnectorManager::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {
	
	return Ok | NoMemClear;
}
