#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

class SubprocessManager;

class SubprocessHandler {
	OT_DECL_NOCOPY(SubprocessHandler)
	OT_DECL_NODEFAULT(SubprocessHandler)
public:
	SubprocessHandler(SubprocessManager* _manager);
	~SubprocessHandler();

private:
	SubprocessManager* m_manager;

};