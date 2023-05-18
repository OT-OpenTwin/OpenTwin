//! @file BlockConnectorManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnectorManager.h"
#include "OpenTwinCore/otAssert.h"

ot::DefaultBlockConnectorManager::DefaultBlockConnectorManager(BlockLayer* _layer) : BlockConnectorManager(_layer) {

}

ot::DefaultBlockConnectorManager::~DefaultBlockConnectorManager() {

}

bool ot::DefaultBlockConnectorManager::addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) {
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

ot::DefaultBlockConnectorManager::QueueResultFlags ot::DefaultBlockConnectorManager::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {
	
	return Ok | NoMemClear;
}
