// Service header
#include "SubprocessHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"

SubprocessHandler::SubprocessHandler(SubprocessManager* _manager)
	: m_manager(_manager)
{
	OTAssertNullptr(m_manager);
}

SubprocessHandler::~SubprocessHandler() {
}
