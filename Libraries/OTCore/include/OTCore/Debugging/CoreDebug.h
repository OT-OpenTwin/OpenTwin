// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Debugging/DebugLogBase.h"

#define OT_CORE_OTOBJECT_DBG_ENABLED false
#if OT_CORE_OTOBJECT_DBG_ENABLED==true
#define OT_CORE_OTOBJECT_DBG(___text)              OT_BASE_DEBUG_LOG("OTOBJECT", ___text)
#define OT_CORE_OTOBJECT_DBG_PTR(___ptr, ___text)  OT_BASE_DEBUG_PTR("OTOBJECT", ___ptr, ___text)
#else
#define OT_CORE_OTOBJECT_DBG(___text)
#define OT_CORE_OTOBJECT_DBG_PTR(___ptr, ___text)
#endif
