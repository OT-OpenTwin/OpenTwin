// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Debugging/DebugLogBase.h"

#define OT_WIDGETS_VIEW_DBG_ENABLED false
#if OT_WIDGETS_VIEW_DBG_ENABLED==true
#define OT_WIDGETS_VIEW_DBG(___text)             OT_BASE_DEBUG_LOG("WIDGETS", ___text)
#define OT_WIDGETS_VIEW_DBG_PTR(___ptr, ___text) OT_BASE_DEBUG_PTR("WIDGETS", ___ptr, ___text)
#else
#define OT_WIDGETS_VIEW_DBG(___text)
#define OT_WIDGETS_VIEW_DBG_PTR(___ptr, ___text)
#endif