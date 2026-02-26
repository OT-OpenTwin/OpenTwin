// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"

#define OT_VIEWER_MEM_DBG_ENABLED false
#if OT_VIEWER_MEM_DBG_ENABLED==true
#define OT_VIEWER_MEM_DBG(___ptr, ___message) OT_LOG_MEM(___ptr, ___message)
#else
#define OT_VIEWER_MEM_DBG(___ptr, ___message)
#endif
