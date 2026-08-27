// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/Logger.h"

#define OT_DEBUG_LOG_PREFIX "[OT][DBG]"
#define OT_BASE_DEBUG_LOG(___lib, ___text) OT_LOG_TS(OT_DEBUG_LOG_PREFIX "[" ___lib "] " << ___text)
#define OT_BASE_DEBUG_PTR(___lib, ___ptr, ___text) OT_LOG_MEMS(___ptr, std::string(OT_DEBUG_LOG_PREFIX "[" ___lib "] "), ___text)
